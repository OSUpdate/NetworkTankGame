#include "Client.h"
#include "GameMap.h"
#include "GameTank.h"
#include "GameControl.h"

#define BUF_SIZE 1024
HANDLE hMutex;

//소켓을 포인터로 참조해 열어줍니다.
int initSock(SOCKET* hSocket,char* ip, int port)
{
	WSADATA	wsaData;
	SOCKADDR_IN serverAddr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		printf("%d WSAStartup() error!", 9000);
	*hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (*hSocket == INVALID_SOCKET)
		printf("%d socket() error", 9000);
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	serverAddr.sin_port = htons(port);
	if (connect(*hSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("연결 실패");
		return 0;
	}
	return 1;
}

//서버에서 보내주는 랜덤 좌표로 초기화해줍니다.
void initTankPos(SOCKET* hSocket)
{
	int bufSize = sizeof(int) * 2, y, x;
	int buf[2];
	recv(*hSocket, (char*)&bufSize, sizeof(int), 0);
	recv(*hSocket, (char*)buf, bufSize, 0);
	y = (buf[1] - GAME_MAP_ORIGIN_Y) / 2;
	x = (buf[0] - GAME_MAP_ORIGIN_X);
	board[y][x] = 2;
	drawTank(buf[0], buf[1]);
	tank.pos.X = buf[0];
	tank.pos.Y = buf[1];

	tank.direction = UP;

	setCurrentCursorPos(buf[0], buf[1]);
}

// 다른 클라의 위치 데이터를 받아옵니다.
void initUser(SOCKET* hSocket)
{
	int bufSize = sizeof(int);
	int buf[2];
	int i,count;
	recv(*hSocket, (char*)&bufSize, sizeof(int), 0);
	recv(*hSocket, (char*)buf, bufSize, 0);
	if (buf[0] != 0)
	{
		count = buf[0];
		for (i = 0; i < count; i++)
		{
			recv(*hSocket, (char*)&bufSize, sizeof(int), 0);
			recv(*hSocket, (char*)buf, bufSize, 0);
			drawEnemyTank(buf[0], buf[1]);
		}
	}
}
//종료시 위치를 전송해 다른 클라이언트에서 지우도록 합니다.
void finishMsg(void)
{
	int dataSize = sizeof(int) * 3;
	int sendData[3] = { FINISH, tank.pos.X, tank.pos.Y};
	send(move,(char*)&dataSize, sizeof(int),0);
	send(move,(char*)sendData,dataSize,0);
}

//맵소켓에 데이터가 왔는지 확인하며 지속적으로 그려주는 스레드입니다.
DWORD WINAPI mapFunc(void)
{
	int dataSize, arrX, arrY,i;
	int recvData[5];
	int data[5];
	int check;
	while (1)
	{
		tank;
		check = recv(map, (char*)&dataSize, sizeof(int), 0);
		if (check == -1)
		{
			setCurrentCursorPos(30, 30);
			printf("서버와 연결이 종료되었습니다.");
			closesocket(missile);
			closesocket(hSocket);
			closesocket(move);
			closesocket(map);
			exit(1);
			return;
		}
		recv(map, (char*)recvData, dataSize, 0);
		WaitForSingleObject(hMutex, INFINITE);
		if (tank.pos.X == recvData[1] && tank.pos.Y == recvData[2])
		{
			
			switch (recvData[0])
			{
			case UP:
				tank.direction = UP;
				deletePos(recvData[1], recvData[2]);
				tank.pos.Y--;
				break;
			case RIGHT:
				tank.direction = RIGHT;
				deletePos(recvData[1], recvData[2]);
				tank.pos.X+=2;
				break;
			case LEFT:
				tank.direction = LEFT;
				deletePos(recvData[1], recvData[2]);
				tank.pos.X-=2;
				break;
			case DOWN:
				tank.direction = DOWN;
				deletePos(recvData[1], recvData[2]);
				tank.pos.Y++;
				break;
			}
			
			drawTank(recvData[3], recvData[4]);
		}
		else
		{
			if (dataSize == sizeof(int) * 2)
			{
				drawEnemyTank(recvData[0], recvData[1]);
			}
			if (dataSize == sizeof(int) * 5)
			{
				deletePos(recvData[1], recvData[2]);
				drawEnemyTank(recvData[3], recvData[4]);
				setCurrentCursorPos(tank.pos.X, tank.pos.Y);
			}
			if (dataSize == sizeof(int) * 3 && (recvData[0] == CRASH || recvData[0] == DEAD))
			{
				deletePos(recvData[1], recvData[2]);
			}
		}
		ReleaseMutex(hMutex);
		Sleep(5);
	}
}
