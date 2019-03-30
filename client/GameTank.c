#include "Client.h"
#include "GameTank.h"
#include "GameMap.h"
#include "GameControl.h"
#include "Common.h"
#include "GameTitle.h"


void deletePos(int x, int y)
{
	
	setCurrentCursorPos(x,y);
	printf("  ");
}
void drawTank(int x, int y)
{
	setCurrentCursorPos(x, y);
	printf("°›");
}
void drawEnemyTank(int x, int y)
{
	setCurrentCursorPos(x, y);
	printf("°‹");
}
void drawVerticalBullet(int x, int y)
{
	setCurrentCursorPos(x, y);
	printf("¶≠");
}
void drawHorizonBullet(int x, int y)
{
	setCurrentCursorPos(x, y);
	printf("¶¨");
}
void drawWall(int x, int y)
{
	setCurrentCursorPos(x, y);
	printf("°‡");
}
DWORD WINAPI missileFunc(void)
{
	int bufSize;
	int buf[5];
	while (1)
	{
		
		recv(missile, (char*)&bufSize, sizeof(int), 0);
		recv(missile, (char*)buf, bufSize, 0);
		WaitForSingleObject(hMutex, INFINITE);
		if (bufSize == sizeof(int) * 3)
		{
			if (buf[0] == UP || buf[0] == DOWN)
			{
				drawVerticalBullet(buf[1], buf[2]);
				continue;
			}
			if (buf[0] == RIGHT || buf[0] == LEFT)
			{
				drawHorizonBullet(buf[1], buf[2]);
				continue;
			}
			if (buf[0] == CRASH)
				deletePos(buf[1], buf[2]);
			if (buf[0] == DEAD)
			{
				deletePos(buf[1], buf[2]);
				//ªÁ∏¡ ∏ﬁΩ√¡ˆ ª—∑¡¡‹
				//finishMsg();
				drawGameOver();
				closesocket(missile);
				closesocket(hSocket);
				closesocket(move);
				closesocket(map);
				return;
			}
			
		}
		if (bufSize != 0)
		{
			if (buf[0] == UP || buf[0] == DOWN)
			{
				deletePos(buf[3], buf[4]);
				drawVerticalBullet(buf[1], buf[2]);
			}
			if (buf[0] == RIGHT || buf[0] == LEFT)
			{
				deletePos(buf[3], buf[4]);
				drawHorizonBullet(buf[1], buf[2]);
			}
			if (buf[0] == 999)
			{
				drawGameWin();
				exit(1);
			}
		}
		ReleaseMutex(hMutex);
	}
}
void leftTank()
{
	int bufSize = sizeof(int) * 3;
	int buf[3];

	setSendData(buf, LEFT);
	tank.direction = LEFT;

	send(move, (char*)&bufSize, sizeof(int), 0);
	send(move, (char*)buf, bufSize, 0);
}
void rightTank()
{
	int bufSize = sizeof(int) * 3;
	int buf[3];

	setSendData(buf, RIGHT);
	tank.direction = RIGHT;

	send(move, (char*)&bufSize, sizeof(int), 0);
	send(move, (char*)buf, bufSize, 0);
}
void upTank()
{
	int bufSize = sizeof(int) * 3;
	int buf[3];

	setSendData(buf,UP);
	tank.direction = UP;

	send(move, (char*)&bufSize, sizeof(int), 0);
	send(move, (char*)buf, bufSize, 0);
}
void downTank()
{
	int bufSize = sizeof(int) * 3;
	int buf[3];

	setSendData(buf, DOWN);
	tank.direction = DOWN;

	send(move, (char*)&bufSize, sizeof(int), 0);
	send(move, (char*)buf, bufSize, 0);
}
void setSendData(int* buf,int dir)
{
	buf[0] = dir;
	buf[1] = tank.pos.X;
	buf[2] = tank.pos.Y;
}
void shootMissile(void)
{
	int dataSize = sizeof(int) * 3;
	int recvData[4] = { SPACE, tank.pos.X, tank.pos.Y };
	send(move, (char*)&dataSize, sizeof(int), 0);
	send(move, (char*)recvData, dataSize, 0);
}
