#ifndef __LOBBY_H__
#define __LOBBY_H__

#include "network.h"
#include "main.h"

#define LOBBYLOGIN_RESULT_ACCEPTED			200
#define LOBBYLOGIN_RESULT_USERNAMEBAD		201
#define LOBBYLOGIN_RESULT_PASSWORDBAD		202
#define LOBBYLOGIN_RESULT_MYSQLERROR		203

class CLobby
{
private:
	dreamClient		*networkClient;
	clientLoginData	*clientList;
	clientLoginData	*localClient;		// Pointer to the local client in the client list

	CArmyWar		*gameList;
	CArmyWar		*localGame;

	int				gameAmount;
	int				timeConnecting;		// How long we have tried to connect

public:
	CLobby();
	~CLobby();

	dreamClient		*GetNetworkClient(void)	{ return networkClient; }
	clientLoginData	*GetLocalClient(void)	{ return localClient; }

	void		RefreshPlayerList(void);
	void		RefreshGameList(void);
	void		RefreshJoinedPlayersList(void);
	
	void		ReadPackets(void);

	void		AddClient(int local, int index, char *name);
	void		RemoveClient(int index);
	void		RemoveClients(void);

	void		AddGame(char *name, int index, bool inProgress);
	void		RemoveGame(char *name);
	void		RemoveGames(void);
	CArmyWar	*GetGamePointer(int ind);

	void		RequestGameData(void);
	void		SendChat(char *text);
	void		SendCreateGame(char *gamename);
	void		SendRemoveGame(char *gamename);
	void		SendStartGame(int ind);
	void		SendKeepAlive(void);

	void		Connect(char *name, char *password);
	void		Disconnect(void);

	void		RunNetwork(int msec);

	int			GetGameAmount(void)				{ return gameAmount; }
	CArmyWar	*GetGameList(void)				{ return gameList; }
	void		SetLocalGame(CArmyWar *game)	{ localGame = game; }
	CArmyWar	*GetLocalGame(void)				{ return localGame; }
};

extern CLobby Lobby;

#endif