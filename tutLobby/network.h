#ifndef NETWORK_H
#define NETWORK_H

#define USER_MES_SERVEREXIT		1
#define USER_MES_LOGIN			2
#define USER_MES_SIGNIN			3
#define USER_MES_CHAT			4
#define USER_MES_CREATEGAME		5
#define USER_MES_REMOVEGAME		6
#define USER_MES_GAMEDATA		7
#define USER_MES_STARTGAME		8
#define USER_MES_MAPDATA		9
#define USER_MES_KEEPALIVE		10

typedef struct clientLoginData
{
	int				index;
	char			nickname[30];
	clientLoginData	*next;
} clientLoginData;

#endif