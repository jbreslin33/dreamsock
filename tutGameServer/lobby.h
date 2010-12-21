#ifndef __LOBBY_H__
#define __LOBBY_H__

#define LOBBY_RESULT_ACCEPTED		200
#define LOBBY_RESULT_USERNAMEBAD	201
#define LOBBY_RESULT_PASSWORDBAD	202
#define LOBBY_RESULT_MYSQLERROR		203

class CLobbyServer
{
private:
	dreamServer		*networkServer;

	clientLoginData	*clientList;
	CArmyWarServer	*gameList;

	int gameAmount;

public:
	CLobbyServer();
	~CLobbyServer();

	int		InitNetwork(void);
	void	ShutdownNetwork(void);

	void	ReadPackets(void);
	void	SendExitNotification(void);

	void	AddClient(void);
	void	RemoveClient(struct sockaddr *address);
	void	RemoveClients(void);

	void	AddGame(char *name);
	void	RemoveGame(char *name);
	void	RemoveGames(void);
	CArmyWarServer *GetGamePointer(int ind);

	void	Frame(int msec);

	int		GetGameAmount(void)			{ return gameAmount; }
	CArmyWarServer *GetGameList(void)	{ return gameList; }
};

#endif
