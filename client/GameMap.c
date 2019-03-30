#include "Common.h"
#include "GameControl.h"
#include "GameMap.h"
#include "GameTank.h"
int board[GAME_MAP_HEIGHT + 2][GAME_MAP_WIDTH + 2] = { 0 };

void initMap(void)
{
	int y, x;
	for (y = 0; y < GAME_MAP_HEIGHT + 2; y++)
	{
		board[y][0] = 1;
		board[y][GAME_MAP_WIDTH + 1] = 1;
	}
	for (x = 0; x < GAME_MAP_WIDTH + 2; x++)
	{
		board[0][x] = 1;
		board[GAME_MAP_HEIGHT + 1][x] = 1;
	}
}
void drawGameMap(void)
{
	system("cls");
	system("mode con: cols=90 lines=30");
	int x, y;
	for (y = 0; y <= GAME_MAP_HEIGHT; y++)
	{
		setCurrentCursorPos(GAME_MAP_ORIGIN_X, GAME_MAP_ORIGIN_Y + y);

		if (y == GAME_MAP_HEIGHT)
			printf("曲");
		else if (y == 0)
			printf("旨");
		else
			printf("早");
	}

	for (y = 0; y <= GAME_MAP_HEIGHT; y++)
	{
		setCurrentCursorPos(GAME_MAP_ORIGIN_X + (GAME_MAP_WIDTH + 1) * 2, GAME_MAP_ORIGIN_Y + y);

		if (y == GAME_MAP_HEIGHT)
			printf("旭");
		else if (y == 0)
			printf("旬");
		else
			printf("早");
	}
	for (x = 1; x < GAME_MAP_WIDTH + 1; x++)
	{
		setCurrentCursorPos(GAME_MAP_ORIGIN_X + x * 2, GAME_MAP_ORIGIN_Y);
		printf("收");

	}
	for (x = 1; x < GAME_MAP_WIDTH + 1; x++)
	{
		setCurrentCursorPos(GAME_MAP_ORIGIN_X + x * 2, GAME_MAP_ORIGIN_Y + GAME_MAP_HEIGHT);
		printf("收");

	}
	setCurrentCursorPos(GAME_MAP_ORIGIN_X, GAME_MAP_ORIGIN_Y);

	initMap();
}
