#ifndef __GAME_TANK_H_
#define __GAME_TANK_H_
#include <Windows.h>
#include <process.h>
typedef struct Player
{
	COORD pos;
	char direction;
} Player;

Player tank;

HANDLE hMutex;

DWORD WINAPI missileFunc(void);
void deletePos(int x, int y);
void drawTank(int x, int y);
void drawEnemyTank(int x, int y);
void setSendData(int* buf, int dir);
void drawHorizonBullet(int x, int y);
void drawVerticalBullet(int x, int y);
void drawWall(int x, int y);
void leftTank(void);
void rightTank(void);
void upTank(void);
void downTank(void);
void shootMissile(void);

#endif // !__GAME_TANK_H_
