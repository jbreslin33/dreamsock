#ifndef __SIGNIN_H__
#define __SIGNIN_H__

#include "network.h"

#define SIGNIN_RESULT_ACCEPTED		200
#define SIGNIN_RESULT_USERNAMEBAD	201
#define SIGNIN_RESULT_PASSWORDBAD	202
#define SIGNIN_RESULT_MYSQLERROR	203

class CSigninServer
{
private:
	dreamServer			*networkServer;
	clientLoginData		*clientList;

public:
	CSigninServer();
	~CSigninServer();

	int		InitNetwork(void);
	void	ShutdownNetwork(void);

	void	ReadPackets(void);
	void	SendExitNotification(void);

	void	AddClient(void);
	void	RemoveClient(struct sockaddr *address);
	void	RemoveClients(void);

	void	Frame(int msec);
};

#endif
