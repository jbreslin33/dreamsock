#ifndef __LOBBY_H__
#define __LOBBY_H__

class CLobbyServer
{
private:
	dreamServer		*networkServer;

	clientData		*clientList;
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

	int		GetGameAmount(void)		{ return gameAmount; }
	CArmyWarServer *GetGameList(void) { return gameList; }
};

#endif