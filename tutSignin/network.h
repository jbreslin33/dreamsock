#ifndef NETWORK_H
#define NETWORK_H

#define USER_MES_SERVEREXIT		1
#define USER_MES_SIGNIN			2
#define USER_MES_KEEPALIVE		3

typedef struct clientLoginData
{
	int				index;
	char			nickname[30];
	clientLoginData	*next;
} clientLoginData;

#endif