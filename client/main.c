#include "Common.h"
#include <Ws2tcpip.h>
#include "GameControl.h"
#include "GameMap.h"
#include "GameTank.h"
#include "GameTitle.h"

char* inputIp()
{
	char* temp = malloc(sizeof(char) * 16);
	char ip[16];
	SOCKADDR_IN	servAdr;
	printf(">> Client IP : ");
	gets(ip);
	strcpy(temp, ip);
	if (!inet_pton(AF_INET, ip, &servAdr.sin_addr))
	{
		printf("\n잘못된 값을 입력하셨습니다.\n\n");
		free(temp);
		temp = inputIp();
	}
	return temp;
}

void main()
{
	char* ip = inputIp();
 	titleMenu(ip);
}
