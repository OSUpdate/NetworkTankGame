#include "Client.h"
#include "GameMap.h"
#include "GameTank.h"
#include "GameControl.h"

#define BUF_SIZE 1024
HANDLE hMutex;

//������ �����ͷ� ������ �����ݴϴ�.
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
		printf("���� ����");
		return 0;
	}
	return 1;
}

//�������� �����ִ� ���� ��ǥ�� �ʱ�ȭ���ݴϴ�.
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

// �ٸ� Ŭ���� ��ġ �����͸� �޾ƿɴϴ�.
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
//����� ��ġ�� ������ �ٸ� Ŭ���̾�Ʈ���� ���쵵�� �մϴ�.
void finishMsg(void)
{
	int dataSize = sizeof(int) * 3;
	int sendData[3] = { FINISH, tank.pos.X, tank.pos.Y};
	send(move,(char*)&dataSize, sizeof(int),0);
	send(move,(char*)sendData,dataSize,0);
}

//�ʼ��Ͽ� �����Ͱ� �Դ��� Ȯ���ϸ� ���������� �׷��ִ� �������Դϴ�.
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
			printf("������ ������ ����Ǿ����ϴ�.");
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
