#ifndef __SIGNIN_H__
#define __SIGNIN_H__

#include "network.h"
#include "main.h"

#define SIGNIN_RESULT_ACCEPTED			200
#define SIGNIN_RESULT_USERNAMEBAD		201
#define SIGNIN_RESULT_PASSWORDBAD		202
#define SIGNIN_RESULT_MYSQLERROR		203

class CSignin
{
private:
	dreamClient		*networkClient;
	clientLoginData	*clientList;
	clientLoginData	*localClient;	// Pointer to the local client in the client list

public:
	CSignin();
	~CSignin();

	dreamClient		*GetNetworkClient(void)	{ return networkClient; }
	clientLoginData	*GetLocalClient(void)	{ return localClient; }

	void	ReadPackets(void);

	void	AddClient(int local, int index, char *name);
	void	RemoveClient(int index);
	void	RemoveClients(void);

	void	SendSignIn(char *nickname, char *firstname,
						char *surname, int age, char *gender, char *password, char *email);
	void	SendKeepAlive(void);

	void	Connect(char *name);
	void	Disconnect(void);

	void	RunNetwork(int msec);
};

extern CSignin Signin;

#endif