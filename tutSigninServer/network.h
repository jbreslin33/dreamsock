#ifndef NETWORK_H
#define NETWORK_H

#include "dreamSock.h"

#define USER_MES_SERVEREXIT		1
#define USER_MES_SIGNIN			2

typedef struct clientLoginData
{
	struct sockaddr		address;
	dreamClient			*netClient;
	clientLoginData		*next;
} clientLoginData;

#endif