#ifndef __CLIENT_H_
#define __CLIENT_H_
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>

#define MAIN_SOCKET 9000
#define MOVE_SOCKET 9010
#define MAP_SOCKET 9020
#define MISSILE_SOCKET 9030

void finishMsg(void);
SOCKET move, map, hSocket, missile;
int initSock(SOCKET* hSocket,char* ip, int port);
void initTankPos(SOCKET* hSocket);
DWORD WINAPI mapFunc(void);
void initUser(SOCKET* hSocket);
#endif