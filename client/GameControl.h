#ifndef __GAME_CONTROL_H_
#define __GAME_CONTROL_H_

#include <Windows.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32
#define DEAD 99
#define CRASH 90
#define FINISH 91

void setCurrentCursorPos(int x, int y);
void removeCursor(void);
COORD getCurrentCursorPos(void);
void pressKeyInput();

#endif // !__GAME_CONTROL_H_
