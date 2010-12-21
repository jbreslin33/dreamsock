#ifndef NETWORK_H
#define NETWORK_H

#define COMMAND_HISTORY_SIZE	64

#define KEY_UP					1
#define KEY_DOWN				2
#define KEY_LEFT				4
#define KEY_RIGHT				8
#define KEY_WEAPON				16

#define CMD_KEY					1
#define CMD_HEADING				2
#define CMD_ORIGIN				4
#define CMD_BULLET				8
#define CMD_FLAG				16
#define CMD_KILL				32

#define USER_MES_FRAME			1
#define USER_MES_NONDELTAFRAME	2
#define USER_MES_SERVEREXIT		3
#define USER_MES_LOGIN			4
#define USER_MES_SIGNIN			5
#define USER_MES_CHAT			6
#define USER_MES_CREATEGAME		7
#define USER_MES_REMOVEGAME		8
#define USER_MES_GAMEDATA		9
#define USER_MES_STARTGAME		10
#define USER_MES_MAPDATA		11
#define USER_MES_KEEPALIVE		12

typedef struct clientLoginData
{
	int					index;
	char				nickname[30];
	clientLoginData	*next;
} clientLoginData;

#endif