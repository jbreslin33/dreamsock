#ifndef NETWORK_H
#define NETWORK_H

#include "dreamSock.h"

#define USER_MES_SERVEREXIT			1
#define USER_MES_LOGIN				2
#define USER_MES_SIGNIN				3
#define USER_MES_CHAT				4
#define USER_MES_CREATEGAME			5
#define USER_MES_REMOVEGAME			6

typedef struct clientLoginData
{
	struct sockaddr		address;
	dreamClient			*netClient;
	clientLoginData		*next;
} clientLoginData;

#endif