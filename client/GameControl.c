#include "Client.h"
#include "GameControl.h"
#include "GameTank.h"


#define SENSITIVE 100
#define SYS_DELAY 20
#define DELAY 5

void removeCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void setCurrentCursorPos(int x, int y)
{
	COORD Pos = { (SHORT)x , (SHORT)y };

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

/*
cmd ȭ�鿡 Ŀ���� �ִ� ��ġ�� ��ȯ�մϴ�.
*/
COORD getCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}
//Ű���� �Է� �Լ��Դϴ�.
void pressKeyInput(void)
{
	char key;
	BOOL bool;
	for (int i = 0; i < SENSITIVE; i++)
	{
		if (kbhit())
		{

			key = getch();
			switch (key)
			{
			case LEFT:
				leftTank();
				break;
			case RIGHT:
				rightTank();
				break;
			case UP:
				upTank();
				break;
			case DOWN:
				downTank();
				break;
			case SPACE:
				shootMissile();
				Sleep(100);
				break;
			}
			Sleep(20);
		}
		if (i % DELAY == 0)
			Sleep(SYS_DELAY);
	}
}