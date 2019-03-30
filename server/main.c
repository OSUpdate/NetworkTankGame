#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
#define BUF_SIZE 1024
#define GAME_MAP_ORIGIN_X 4
#define GAME_MAP_ORIGIN_Y 2
#define GAME_MAP_WIDTH 30
#define GAME_MAP_HEIGHT 20
#define MAX_CLIENT 5

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
#define DEAD 99
#define CRASH 90
#define FINISH 91
#define WIN 999
typedef struct Point
{
	int X;
	int Y;
} Point;
typedef struct Player
{
	Point pos;
	char dir;
} PLAYER;

typedef struct Missile
{
	Point pre;
	Point pos;
	Point arr;
} Missile;

typedef struct SOCK
{
	SOCKET hSock;
	SOCKADDR_IN Addr;
	PLAYER tank;
} SOCK;


HANDLE hMutex;

int map[GAME_MAP_HEIGHT+ 2][GAME_MAP_WIDTH + 2] = { 0 };
int currentCount = 0;
int count = 0;

SOCK moveClient[MAX_CLIENT];
SOCK missileClient[MAX_CLIENT];
SOCK mapClient[MAX_CLIENT];
SOCK client[MAX_CLIENT];

void ErrorHandling(char* message, int port);
void initMap(void);
int isCollision(int arrX, int arrY);
Point toCoord(int x, int y);
int upTank(int x, int y, int index);
int downTank(int x, int y, int index);
int rightTank(int x, int y, int index);
int leftTank(int x, int y, int index);
void initSock(SOCK* hSocket, int port);
void sendMap(int *sendData);
int moveConnect(SOCK* move, int index);
int mapConnect(SOCK* map, int index);
DWORD WINAPI moveFunc(void* arg);
int findTank(int index, int posX, int posY);
void crashMsg(Missile* missile);
void initMissile(Missile* temp, int posX, int posY);
void sendMissile(Missile* missile, int dir);
Point initTank(SOCK* socket, int index);
void initUser(SOCK* socket, Point *point);
DWORD WINAPI missileFunc(void* arg);
void setClose(int index, Missile* missile);
void main()
{
	HANDLE hThread;
	DWORD threadId;
	WSADATA	wsaData;
	Point initPos;
	SOCK server;
	int check;
	int szClntAddr;
	int dataSize = sizeof(int);
	SOCK moveSocket, mapSocket, missileSocket;
	int sendData = 101, error = 102;
	int i;
	int index = 0;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	initMap();
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!", 9000);
	initSock(&server, 9000);
	initSock(&moveSocket, 9010);
	initSock(&mapSocket, 9020);
	initSock(&missileSocket, 9030);


	while (1)
	{
		if (index < 2)
		{
			szClntAddr = sizeof(client[index].Addr);
			client[index].hSock = accept(server.hSock, (SOCKADDR*)&client[index].Addr, &szClntAddr);
			if (client[index].hSock == INVALID_SOCKET)
				ErrorHandling("accept() error", 9000);
		
			
			initPos = initTank(&client[index], index);
			initUser(&client[index], &initPos);
			printf("ip : %s port : %d 연결 성공\n", inet_ntoa(client[index].Addr.sin_addr), client[index].Addr.sin_port);
		
			if (moveConnect(&moveSocket, index))
			{
				if (mapConnect(&mapSocket, index))
				{
					if (missileConnect(&missileSocket, index))
					{
						hThread = CreateThread(NULL, 0, moveFunc, (void*)index, 0, &threadId);//스레드 시작 지점
						index++;
						
						currentCount++;
						ReleaseMutex(hMutex);
						if (index > 1)
						{
							for (i = 0; i < currentCount; i++)
							{
								send(client[i].hSock, (char*)&dataSize, sizeof(int), 0);
								send(client[i].hSock, (char*)&sendData, dataSize, 0);
							}
						}
					}

				}
			}
		}
		if (count == 1)
		{
			for (i = 0; i < currentCount; i++)
			{
				send(missileClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
				sendData = WIN;
				send(missileClient[i].hSock, (char*)&sendData,dataSize, 0);
				
			}
			Sleep(100);
			return;
		}
	}
}


void initUser(SOCK* socket, Point *point)
{
	int dataSize = sizeof(int), i;
	int sendData[2];
	int check;
	check = send(socket->hSock, (char*)&dataSize, sizeof(int), 0);
	if (check == 0)
		closesocket(socket->hSock);

	send(socket->hSock, (char*)&currentCount, dataSize, 0);
	if (currentCount != 0)
	{
		for (i = 0; i < currentCount; i++)
		{
			dataSize = sizeof(int) * 2;
			if (moveClient[i].tank.pos.X == 0 && moveClient[i].tank.pos.Y == 0)
				continue;
			sendData[0] = moveClient[i].tank.pos.X;
			sendData[1] = moveClient[i].tank.pos.Y;
			check = send(socket->hSock, (char*)&dataSize, sizeof(int), 0);
			if (check == 0)
				closesocket(socket->hSock);
			send(socket->hSock, (char*)sendData, dataSize, 0);
		}
		for (i = 0; i < currentCount; i++)
		{
			dataSize = sizeof(int) * 2;
			sendData[0] = point->X;
			sendData[1] = point->Y;
			check = send(mapClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
			if (check == 0)
				closesocket(socket->hSock);
			send(mapClient[i].hSock, (char*)sendData, dataSize, 0);
		}
	}
}

Point toCoord(int x, int y)
{
	Point temp;
	temp.X = (x - GAME_MAP_ORIGIN_X) / 2;
	temp.Y = (y - GAME_MAP_ORIGIN_Y);
	return temp;
}
void initMap(void)
{
	int y, x;
	for (y = 0; y < GAME_MAP_HEIGHT + 2; y++)
	{
		map[y][0] = 1;
		map[y][GAME_MAP_WIDTH + 1] = 1;
	}
	for (x = 0; x < GAME_MAP_WIDTH + 2; x++)
	{
		map[0][x] = 1;
		map[GAME_MAP_HEIGHT][x] = 1;
	}
}

int isCollision(int posX, int posY)
{
	Point arr = toCoord(posX, posY);
	if (map[arr.Y][arr.X] == 1 || map[arr.Y][arr.X] == 2)
		return 0;
	return 1;
}
int isMissileCollision(int posX, int posY)
{
	Point arr = toCoord(posX, posY);
	if (map[arr.Y][arr.X] == 1 || map[arr.Y][arr.X] == 3)
		return 0;
	return 1;
}
int isTankCollision(int posX, int posY)
{
	Point arr = toCoord(posX, posY);
	if (map[arr.Y][arr.X] == 2)
		return 0;
	return 1;
}
int upTank(int x, int y,int index)
{

	Point arr = toCoord(x, y);
	//충돌 확인 함수
	if (!isCollision(x, y - 1))
		return 0;

	map[arr.Y - 1][arr.X] = 2;
	map[arr.Y][arr.X] = 0;
	moveClient[index].tank.pos.Y--;
	return 1; // 성공적으로 이동했을경우

}
int downTank(int x, int y, int index)
{

	Point arr = toCoord(x, y);
	//충돌 확인 함수 생성
	if (!isCollision(x, y + 1))
		return 0;
	map[arr.Y + 1][arr.X] = 2;
	map[arr.Y][arr.X] = 0;
	moveClient[index].tank.pos.Y++;
	return 1;
	 // 성공적으로 이동했을경우
	
}
int rightTank(int x, int y, int index)
{
	Point arr = toCoord(x, y);


	//충돌 확인 함수 생성
	if (!isCollision(x + 2, y))
		return 0;
	map[arr.Y][arr.X + 1] = 2;
	map[arr.Y][arr.X] = 0;
	moveClient[index].tank.pos.X+=2;

	return 1; // 성공적으로 이동했을경우

}
int leftTank(int x, int y, int index)
{
	Point arr = toCoord(x, y);
	//충돌 확인 함수 생성
	if (!isCollision(x - 2, y))
		return 0;
	map[arr.Y][arr.X - 1] = 2;
	map[arr.Y][arr.X] = 0;
	moveClient[index].tank.pos.X-=2;

	return 1; // 성공적으로 이동했을경우

}
void ErrorHandling(char* message,int port)
{
	printf("port %d %s \n", port, message);
}

void initSock(SOCK* hSocket,int port)
{
	int option = 1;
	hSocket->hSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&hSocket->Addr, 0, sizeof(hSocket->Addr));
	hSocket->Addr.sin_family = AF_INET;
	hSocket->Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	hSocket->Addr.sin_port = htons(port);
	setsockopt(hSocket->hSock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
	if (bind(hSocket->hSock, (SOCKADDR*)&hSocket->Addr, sizeof(hSocket->Addr)) == SOCKET_ERROR)
		ErrorHandling("bind() error", port);
	if (listen(hSocket->hSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error", port);
}
void sendMap(int *sendData)
{
	//데이터 크기 계산
	int dataSize = sizeof(int) * 5;
	int index;
	for (index = 0; index < currentCount; index++)
	{
		//전송 데이터는 recvData에 있는 좌표값을 계산해서 다시 보내주어도 됨
		send(mapClient[index].hSock, (char*)&dataSize, sizeof(int), 0);
		send(mapClient[index].hSock, (char*)sendData, sizeof(int) * dataSize, 0);

	}
}
void setMoveData(int dir, int* sendData, int x,int y ,int index)
{
	sendData[0] = dir;
	sendData[1] = x;
	sendData[2] = y;
	sendData[3] = moveClient[index].tank.pos.X;
	sendData[4] = moveClient[index].tank.pos.Y;
}
void closeUser(int index)
{
	int i,dataSize = sizeof(int) * 3;
	int sendData[3];
	Point arr = toCoord(moveClient[index].tank.pos.X, moveClient->tank.pos.Y);
	sendData[0] = DEAD;
	sendData[1] = moveClient[index].tank.pos.X;
	sendData[2] = moveClient[index].tank.pos.Y;
	map[arr.Y][arr.X] = 0;
	for (i = 0; i < currentCount; i++)
	{
		if (i == index)
			continue;
		send(mapClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
		send(mapClient[i].hSock, (char*)sendData, dataSize, 0);
	}
}
int missileConnect(SOCK* missile, int index)
{
	int sockSize = sizeof(missileClient[index].Addr);
	missileClient[index].hSock = accept(missile->hSock, (SOCKADDR*)&missile->Addr, &sockSize);
	if (missileClient[index].hSock == INVALID_SOCKET)
	{
		ErrorHandling("accept() error", 9010);
		return 0;
	}
	return 1;
}
int moveConnect(SOCK* move,int index)
{
	int sockSize = sizeof(moveClient[index].Addr);
	moveClient[index].hSock = accept(move->hSock, (SOCKADDR*)&move->Addr, &sockSize);
	if (moveClient[index].hSock == INVALID_SOCKET)
	{
		ErrorHandling("accept() error", 9010);
		return 0;
	}
	return 1;
}
int mapConnect(SOCK* map, int index)
{
	int sockSize = sizeof(mapClient[index].Addr);
	mapClient[index].hSock = accept(map->hSock, (SOCKADDR*)&map->Addr, &sockSize);
	if (mapClient[index].hSock == INVALID_SOCKET)
	{
		ErrorHandling("accept() error", 9020);
		return 0;
	}
	return 1;
}
void initMissile(Missile* temp, int posX, int posY)
{
	Point point = toCoord(posX, posY);
	temp->pre.X = 0;
	temp->pre.Y = 0;
	temp->pos.X = posX;
	temp->pos.Y = posY;
	temp->arr.X = point.X;
	temp->arr.Y = point.Y;
}
int findTank(int index, int posX, int posY)
{
	int i;
	int dataSize = sizeof(int) * 3;
	int sendData[3] = {DEAD,posX,posY};
	for (i = 0; i < currentCount; i++)
	{
		if (moveClient[i].tank.pos.X == posX &&
			moveClient[i].tank.pos.Y == posY)
		{
			send(missileClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
			send(missileClient[i].hSock, (char*)sendData, dataSize, 0);
			return i;
		}
	}
}

//미사일 데이터를 세팅하고 전송해줍니다.
void sendMissile(Missile* missile, int dir)
{
	int i;
	int firstDataSize = sizeof(int) *3;
	int dataSize = sizeof(int) * 5;
	int sendData[5] = {dir,missile->pos.X, missile->pos.Y, missile->pre.X, missile->pre.Y};
	//뒤에 미사일이 없을 경우 실행됨. (미사일 위치는 탱크 바로 앞)
	if (missile->pre.X == 0 && missile->pre.Y == 0)
	{
		for (i = 0; i < currentCount; i++)
		{
			send(missileClient[i].hSock, (char*)&firstDataSize, sizeof(int), 0);
			send(missileClient[i].hSock, (char*)sendData, dataSize, 0);
		}
		return;
	}
	
	for (i = 0; i < currentCount; i++)
	{
		send(missileClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
		send(missileClient[i].hSock, (char*)sendData, dataSize, 0);
	}

}	
//벽,탱크와 미사일이 충돌시 마지막에 남은 미사일을 지워주기 위한 메시지
void crashMsg(Missile* missile)
{
	int i;
	int dataSize = sizeof(int) * 3;
	int sendData[3] = { CRASH,missile->pos.X,missile->pos.Y };
	for (i = 0; i < currentCount; i++)
	{
		send(missileClient[i].hSock, (char*)&dataSize, sizeof(int), 0);
		send(missileClient[i].hSock, (char*)sendData, sizeof(int), 0);
	}
	
}
//미사일 스레드, 스페이스바를 누를시 생성되며 벽이나 탱크에 닿기 전까지 반복됨
DWORD WINAPI missileFunc(void * arg)
{
	Missile missile;
	Point pos;
	int deadUser;
	int index = (int)arg;
	int dataSize = sizeof(int) * 5;
	int first = 1;
	int sendData[5];
	int dir = moveClient[index].tank.dir;
	switch (dir)
	{
	case UP:
		
		initMissile(&missile, moveClient[index].tank.pos.X, moveClient[index].tank.pos.Y - 1);
		while (1)
		{
			Sleep(20);
			WaitForSingleObject(hMutex, INFINITE);
			// 미사일이 벽, 미사일에 닿았는지 체크
			if (!isMissileCollision(missile.pos.X, missile.pos.Y))
			{
 	 			crashMsg(&missile);
				map[missile.arr.Y + 1][missile.arr.X] = 0;
				return;
			}
			//미사일과 충돌시 해당 위치의 유저 찾아서 사망 메시지 전송합니다.
			if (!isTankCollision(missile.pos.X, missile.pos.Y))
			{
				deadUser = findTank(index, missile.pos.X, missile.pos.Y);
				crashMsg(&missile);
				Sleep(20);
				closeUser(deadUser);

				setClose(deadUser, &missile);
				
				return;
			}
			if (map[missile.arr.Y + 1][missile.arr.X] != 2)
				map[missile.arr.Y + 1][missile.arr.X] = 0;
			map[missile.arr.Y][missile.arr.X] = 3;
			sendMissile(&missile, dir);
			missile.pre.X = missile.pos.X;
			missile.pre.Y = missile.pos.Y;
			missile.pos.Y -= 1;
			missile.arr.Y -= 1;
			ReleaseMutex(hMutex);
		}
		break;
	case RIGHT:
		initMissile(&missile, moveClient[index].tank.pos.X + 2, moveClient[index].tank.pos.Y);
		while (1)
		{
			Sleep(20);
			WaitForSingleObject(hMutex, INFINITE);
			if (!isMissileCollision(missile.pos.X, missile.pos.Y))
			{
				crashMsg(&missile);
				map[missile.arr.Y][missile.arr.X - 1] = 0;
				return;
			}
 			if (!isTankCollision(missile.pos.X, missile.pos.Y))
			{

				deadUser = findTank(index, missile.pos.X, missile.pos.Y);
				crashMsg(&missile);
				Sleep(20);
				closeUser(deadUser);

				setClose(deadUser, &missile);
				return;
			}
			if (map[missile.arr.Y][missile.arr.X - 1] != 2)
				map[missile.arr.Y][missile.arr.X - 1] = 0;
			map[missile.arr.Y][missile.arr.X] = 3;
			sendMissile(&missile, dir);
			missile.pre.X = missile.pos.X;
			missile.pre.Y = missile.pos.Y;
			missile.pos.X += 2;
			missile.arr.X += 1;
			ReleaseMutex(hMutex);
		}
		break;
	case LEFT:
		initMissile(&missile, moveClient[index].tank.pos.X - 2, moveClient[index].tank.pos.Y);
		while (1)
		{
			Sleep(20);
			WaitForSingleObject(hMutex, INFINITE);
			if (!isMissileCollision(missile.pos.X, missile.pos.Y))
			{
				crashMsg(&missile);
				map[missile.arr.Y][missile.arr.X + 1] = 0;
				return;
			}
			if (!isTankCollision(missile.pos.X, missile.pos.Y))
			{
				deadUser = findTank(index, missile.pos.X, missile.pos.Y);
				crashMsg(&missile);
				Sleep(20);
				closeUser(deadUser);

				setClose(deadUser, &missile);
				
				return;
			}
			if (map[missile.arr.Y][missile.arr.X + 1] != 2)
				map[missile.arr.Y][missile.arr.X + 1] = 0;
			map[missile.arr.Y][missile.arr.X] = 3;
			sendMissile(&missile, dir);
			missile.pre.X = missile.pos.X;
			missile.pre.Y = missile.pos.Y;
			missile.pos.X -= 2;
			missile.arr.X -= 1;
			ReleaseMutex(hMutex);
		}
		break;
	case DOWN:
		initMissile(&missile, moveClient[index].tank.pos.X, moveClient[index].tank.pos.Y + 1);
		while (1)
		{
			Sleep(20);
			WaitForSingleObject(hMutex, INFINITE);
			if (!isMissileCollision(missile.pos.X, missile.pos.Y))
			{
				crashMsg(&missile);
				map[missile.arr.Y - 1][missile.arr.X] = 0;
				return;
			}
			if (!isTankCollision(missile.pos.X, missile.pos.Y))
			{
				deadUser = findTank(index, missile.pos.X, missile.pos.Y);
				crashMsg(&missile);
				Sleep(20);
				closeUser(deadUser);

				setClose(deadUser,&missile);
				
				return;
			}
			if (map[missile.arr.Y - 1][missile.arr.X] != 2)
				map[missile.arr.Y - 1][missile.arr.X] = 0;
			map[missile.arr.Y][missile.arr.X] = 3;
			sendMissile(&missile, dir);
			missile.pre.X = missile.pos.X;
			missile.pre.Y = missile.pos.Y;
			missile.pos.Y += 1;
			missile.arr.Y += 1;
			ReleaseMutex(hMutex);
		}
		break;
	}
}

//탱크과 미사일이 충돌한 경우,
//해당 클라이언트를 닫아주기 위한 설정입니다.
void setClose(int index, Missile* missile)
{
	moveClient[index].tank.pos.X = 0;
	moveClient[index].tank.pos.Y = 0;
	closesocket(moveClient[index].hSock);
	closesocket(mapClient[index].hSock);
	closesocket(missileClient[index].hSock);
	closesocket(client[index].hSock);
	map[missile->arr.Y][missile->arr.X] = 0;
	map[missile->arr.Y - 1][missile->arr.X] = 0;

}
//탱크 주변 어디에 미사일이 존재하는지 체크합니다.
int checkMissile(int x, int y, int index)
{
	Point arr = toCoord(x, y);
	switch (moveClient[index].tank.dir)
	{
	case UP:
		if (map[arr.Y - 1][arr.X] == 1 || map[arr.Y - 1][arr.X] == 3)
			return 0;
		break;
	case RIGHT:
		if (map[arr.Y][arr.X + 1] == 1 || map[arr.Y][arr.X + 1] == 3)
			return 0;
		break;
	case LEFT:
		if (map[arr.Y][arr.X - 1] == 1 || map[arr.Y][arr.X - 1] == 3)
			return 0;
		break;
	case DOWN:
		if (map[arr.Y + 1][arr.X] == 1 || map[arr.Y + 1][arr.X] == 3)
			return 0;
		break;
	}
	return 1;
}

//처음 탱크 위치를 랜덤으로 초기화해줍니다.
Point initTank(SOCK* socket, int index)
{
	Point point;

	srand((unsigned int)time(NULL));
	int x = rand() % (GAME_MAP_WIDTH * 2) + GAME_MAP_ORIGIN_X + 2;
	int y = rand() % GAME_MAP_HEIGHT + GAME_MAP_ORIGIN_Y;
	Point arr = toCoord(x, y);
	int dataSize = sizeof(int) * 2;
	int check;
	int sendData[2];

	while (x % 2 == 1 || map[arr.Y][arr.X] == 1 || map[arr.Y][arr.X] == 2)
	{
		x = rand() % (GAME_MAP_WIDTH * 2) + GAME_MAP_ORIGIN_X + 2;
		y = rand() % GAME_MAP_HEIGHT + GAME_MAP_ORIGIN_Y;
		arr = toCoord(x, y);
	}
	map[arr.Y][arr.X] = 2;
	sendData[0] = x;
	sendData[1] = y;
	moveClient[index].tank.pos.X = x;
	moveClient[index].tank.pos.Y = y;
	point.X = x;
	point.Y = y;
	check = send(socket->hSock, (char*)&dataSize, sizeof(int), 0);
	if (check == 0)
		closesocket(socket->hSock);
	send(socket->hSock, (char*)sendData, dataSize, 0);
	return point;
}
//이동 소켓을 처리하는 스레드
DWORD WINAPI moveFunc(void* arg)
{
	int i, dataSize;
	Point temp;
	int index = (int)arg;
	int recvData[5] = { 0 };
	int sendData[5] = { 0 };
	int x,y,check;
	HANDLE hTrd;
	DWORD threadId;
	while (1)
	{
		
		//데이터를 담아야함
		check = recv(moveClient[index].hSock, (char*)&dataSize, sizeof(int), 0);
 		if (check == -1)
		{
			printf("ip : %s port : %d 연결 종료\n", inet_ntoa(client[index].Addr.sin_addr),ntohs(client[index].Addr.sin_port));
			closeUser(index);
			count++;
			closesocket(moveClient[index].hSock);
			closesocket(mapClient[index].hSock);
			closesocket(client[index].hSock);
			return;
		}
		recv(moveClient[index].hSock, (char*)recvData, dataSize, 0);
		x = recvData[1];
		y = recvData[2];
		switch (recvData[0])
		{
		case UP:
			if (upTank(x, y, index))
			{
				moveClient[index].tank.dir = UP;
				setMoveData(UP, sendData, x, y, index);
				//모든 클라에 위치 데이터 전송
				sendMap(sendData);
			}
			break;
		case RIGHT:
			if (rightTank(x, y, index))
			{
				moveClient[index].tank.dir = RIGHT;
				setMoveData(RIGHT, sendData, x, y, index);
				sendMap(sendData);
			}
			break;
		case LEFT:
			if (leftTank(x, y, index))
			{
				moveClient[index].tank.dir = LEFT;
				setMoveData(LEFT, sendData, x, y, index);
				sendMap(sendData);
			}
			break;
		case DOWN:
			if (downTank(x, y, index))
			{
				moveClient[index].tank.dir = DOWN;
				setMoveData(DOWN, sendData, x, y, index);
				sendMap(sendData);
			}
			break;
			//미사일을 발사할 경우
		case SPACE:
			//탱크 상하좌우에 미사일이 없을 경우 스레드 생성
			if (checkMissile(x, y, index))
			{
				
				hTrd = CreateThread(NULL, 0, missileFunc, (void*)index, 0, &threadId);
				Sleep(50);
			}
			break;
		case FINISH:
			Sleep(100);
			temp = toCoord(x, y);
			map[temp.Y][temp.X] = 0;
			moveClient[index].tank.pos.X = 0;
			moveClient[index].tank.pos.Y = 0;
			break;
		}
		
		Sleep(40);
	}
	return;
}


