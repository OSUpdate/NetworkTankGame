#include "Client.h"
#include "GameTitle.h"
#include "GameMap.h"
#include "GameControl.h"
#include "GameTank.h"
#include <process.h>

#define MENU_CURSOR_X 25
#define MENU_CURSOR_Y 13

//메인 화면에서 위 아래 방향키로 조작합니다.
void cursorMenu(char* ip)
{
	char key;
	int pos = MENU_CURSOR_Y;
	while (1) {
		if (kbhit())
		{
			key = getch();
			switch (key)
			{
			case UP:
				if (getCurrentCursorPos().Y == MENU_CURSOR_Y + 2)
					setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y);
				else
					setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y);
				printf(">> 1.시작");
				setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y + 2);
				printf("   2.종료");
				pos = MENU_CURSOR_Y;
				break;
			case DOWN:
				setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y);
				printf("   1.시작");
				if (getCurrentCursorPos().Y == MENU_CURSOR_Y)
					setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y + 2);
				else
					setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y + 2);
				printf(">> 2.종료");
				pos = MENU_CURSOR_Y + 2;
				break;
			case ENTER:
				if (pos == MENU_CURSOR_Y)
					run(ip);
				else
					exit(0);
				break;


			}
		}
	}
}
// 실제 동작되는 함수입니다.
void run(char* ip)
{
	HANDLE missileThread,mapThread;
	DWORD threadId;
	drawGameMap();
	removeCursor();
	int dataSize,data;
	if (!initSock(&hSocket,ip , MAIN_SOCKET))
		printf("error");
	initTankPos(&hSocket);
	initUser(&hSocket);
	//drawTank();
	if (initSock(&move,ip ,MOVE_SOCKET))
	{
		if (initSock(&map,ip , MAP_SOCKET))
		{
			if (initSock(&missile, ip, MISSILE_SOCKET))
			{
				mapThread = CreateThread(NULL, 0, mapFunc, NULL, 0, &threadId);
				missileThread = CreateThread(NULL, 0, missileFunc, NULL, 0, &threadId);
				while (1)
				{
					recv(hSocket, (char*)&dataSize, sizeof(int), 0);
					recv(hSocket, (char*)&data, dataSize, 0);
					if (data == 101)
					{
						while(1)
							pressKeyInput();
					}
						

				}
			}
		}
	}
}
void titleMenu(char* ip)
{

	system("mode con: cols=70 lines=30");
	system("cls");
	printf("\n"
		" ===============================================================\n"
		"\n"
		"            _|_|_|_|_|                    _|        \n"
		"                _|      _|_|_|  _|_|_|    _|  _|    \n"
		"                _|    _|    _|  _|    _|  _|_|      \n"
		"                _|    _|    _|  _|    _|  _|  _|    \n"
		"                _|      _|_|_|  _|    _|  _|    _|  \n"
		"\n"
		" ===============================================================");

	setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y);
	printf(">> 1.시작");
	setCurrentCursorPos(MENU_CURSOR_X + 3, MENU_CURSOR_Y + 2);
	printf("2.종료");
	setCurrentCursorPos(MENU_CURSOR_X, MENU_CURSOR_Y);
	removeCursor();
	cursorMenu(ip);

}
void drawGameOver(void)
{
	system("cls");
	setCurrentCursorPos(13, 5);
	printf("   _|_|_|                                      ");
	setCurrentCursorPos(13, 6);
	printf(" _|          _|_|_|  _|_|_|  _|_|      _|_|    ");
	setCurrentCursorPos(13, 7);
	printf(" _|  _|_|  _|    _|  _|    _|    _|  _|_|_|_|  ");
	setCurrentCursorPos(13, 8);
	printf(" _|    _|  _|    _|  _|    _|    _|  _|        ");
	setCurrentCursorPos(13, 9);
	printf("   _|_|_|    _|_|_|  _|    _|    _|    _|_|_|  ");
	setCurrentCursorPos(13, 11);
	printf("   _|_|                                        ");
	setCurrentCursorPos(13, 12);
	printf(" _|    _|  _|      _|    _|_|    _|  _|_|      ");
	setCurrentCursorPos(13, 13);
	printf(" _|    _|  _|      _|  _|_|_|_|  _|_|          ");
	setCurrentCursorPos(13, 14);
	printf(" _|    _|    _|  _|    _|        _|            ");
	setCurrentCursorPos(13, 15);
	printf( "   _|_|        _|        _|_|_|  _|            ");
	setCurrentCursorPos(13, 17);

}

void drawGameWin(void)
{
	system("cls");
	setCurrentCursorPos(13, 5);
	printf(" _|      _|                    ");
	setCurrentCursorPos(13, 6);
	printf("   _|  _|    _|_|    _|    _|  ");
	setCurrentCursorPos(13, 7);
	printf("     _|    _|    _|  _|    _|  ");
	setCurrentCursorPos(13, 8);
	printf("     _|    _|    _|  _|    _|  ");
	setCurrentCursorPos(13, 9);
	printf("     _|      _|_|      _|_|_|  ");
	setCurrentCursorPos(13, 11);
	printf("          _|          _|  _|_|_|  _|      _|  ");
	setCurrentCursorPos(13, 12);
	printf("          _|          _|    _|    _|_|    _|  ");
	setCurrentCursorPos(13, 13);
	printf("          _|    _|    _|    _|    _|  _|  _|  ");
	setCurrentCursorPos(13, 14);
	printf("            _|  _|  _|      _|    _|    _|_|  ");
	setCurrentCursorPos(13, 15);
	printf("              _|  _|      _|_|_|  _|      _|  ");
	setCurrentCursorPos(13, 17);

}