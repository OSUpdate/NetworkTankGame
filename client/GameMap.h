
#ifndef __GAME_MAP_H_
#define __GAME_MAP_H_

#define GAME_MAP_ORIGIN_X 4
#define GAME_MAP_ORIGIN_Y 2
#define GAME_MAP_WIDTH 30
#define GAME_MAP_HEIGHT 20

extern int board[GAME_MAP_HEIGHT + 2][GAME_MAP_WIDTH + 2];

void initMap(void);
void drawGameMap(void);

#endif // !__GAME_MAP_H_
