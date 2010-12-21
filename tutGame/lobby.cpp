/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game client                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include "resource.h"

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CLobby::CLobby()
{
	networkClient	= new dreamClient;
	clientList		= NULL;
	gameList		= NULL;
	localGame		= NULL;
	gameAmount		= 0;
	timeConnecting	= 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CLobby::~CLobby()
{
	delete networkClient;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RefreshPlayerList(void)
{
	SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_PLAYERLIST), LB_RESETCONTENT, 0, 0);

	clientLoginData *list = clientList;

	for( ; list != NULL; list = list->next)
	{
		SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_PLAYERLIST), LB_ADDSTRING, 0, 
			(LPARAM) list->nickname);
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RefreshGameList(void)
{
	char temp[128];

	SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_GAMELIST), LB_RESETCONTENT, 0, 0);

	CArmyWar *list = gameList;

	for( ; list != NULL; list = list->next)
	{
		strcpy(temp, list->GetName());

		if(list->GetInProgress())
		{
			strcat(temp, " (in progress)");
		}

		SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_GAMELIST), LB_ADDSTRING, 0, 
			(LPARAM) temp);
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RefreshJoinedPlayersList(void)
{
	if(localGame == NULL)
		return;

	SendMessage(GetDlgItem(hWnd_CreateViewPlayersDialog,
		IDC_PLAYERSINGAME), LB_RESETCONTENT, 0, 0);

	SendMessage(GetDlgItem(hWnd_JoinGameDialog,
		IDC_JOINPLAYERSINGAME), LB_RESETCONTENT, 0, 0);

	clientData *list = localGame->GetClientList();

	for( ; list != NULL; list = list->next)
	{
		SendMessage(GetDlgItem(hWnd_CreateViewPlayersDialog,
			IDC_PLAYERSINGAME), LB_ADDSTRING, 0,
			(LPARAM) list->nickname);

		SendMessage(GetDlgItem(hWnd_JoinGameDialog,
			IDC_JOINPLAYERSINGAME), LB_ADDSTRING, 0,
			(LPARAM) list->nickname);
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::ReadPackets(void)
{
	char data[1400];
	struct sockaddr address;

	int type;
	int ind;
	int inProgress;
	int local;
	int ret;
	char name[30];

	dreamMessage mes;
	mes.Init(data, sizeof(data));

	while(ret = networkClient->GetPacket(mes.data, &address))
	{
		mes.SetSize(ret);
		mes.BeginReading();

		type = mes.ReadByte();

		switch(type)
		{
		case DREAMSOCK_MES_ADDCLIENT:
			local	= mes.ReadByte();
			ind		= mes.ReadByte();
			strcpy(name, mes.ReadString());

			AddClient(local, ind, name);
			break;

		case DREAMSOCK_MES_REMOVECLIENT:
			ind = mes.ReadByte();

			LogString("Got removeclient %d message", ind);

			RemoveClient(ind);
			break;

		case USER_MES_SERVEREXIT:
			MessageBox(NULL, "Server disconnected", "Info", MB_OK);
			Disconnect();
			break;

		case USER_MES_LOGIN:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			ret = mes.ReadShort();

			LogString("Got lobby login respond %d", ret);

			if(ret != LOBBYLOGIN_RESULT_ACCEPTED)
			{
				MessageBox(NULL, "Nickname or password is not valid", "Error", MB_OK);

				Disconnect();

				return;
			}

			SetWindowText(hWnd_Application, "ARMY WAR Online 2.0 - connected");
			timeConnecting = -1;

			ShowWindow(hWnd_LobbyDialog, SW_SHOW);

			break;

		case USER_MES_CHAT:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			SendMessage(GetDlgItem(hWnd_LobbyDialog, IDC_CHATLIST), LB_ADDSTRING, 0, 
				(LPARAM) mes.ReadString());

			break;

		case USER_MES_CREATEGAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			// Read game name
			strcpy(name, mes.ReadString());

			ind			= mes.ReadShort();
			inProgress	= mes.ReadByte();

			AddGame(name, ind, inProgress);

			break;

		case USER_MES_REMOVEGAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			// Read game name
			strcpy(name, mes.ReadString());

			LogString("Removing game '%s'", name);

			RemoveGame(name);

			break;

		case USER_MES_STARTGAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			ind = mes.ReadShort();

			if(GetGamePointer(ind))
				GetGamePointer(ind)->SetInProgress(true);

			RefreshGameList();

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::AddClient(int local, int ind, char *name)
{
	// First get a pointer to the beginning of client list
	clientLoginData *list = clientList;
	clientLoginData *prev;

	LogString("App: Client: Adding client with index %d", ind);

	// No clients yet, adding the first one
	if(clientList == NULL)
	{
		LogString("App: Client: Adding first client");

		clientList = (clientLoginData *) calloc(1, sizeof(clientLoginData));

		if(local)
		{
			LogString("App: Client: This one is local");
			localClient = clientList;
		}

		clientList->index = ind;
		strcpy(clientList->nickname, name);
		strcpy(clientList->nickname, name);

		clientList->next = NULL;
	}
	else
	{
		LogString("App: Client: Adding another client");

		prev = list;
		list = clientList->next;

		while(list != NULL)
		{
			prev = list;
			list = list->next;
		}

		list = (clientLoginData *) calloc(1, sizeof(clientLoginData));

		if(local)
		{
			LogString("App: Client: This one is local");
			localClient = list;
		}

		list->index = ind;
		strcpy(list->nickname, name);

		list->next = NULL;

		prev->next = list;
	}

	RefreshPlayerList();

	// If we just joined the game, request all the game data
	if(local)
		RequestGameData();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveClient(int ind)
{
	clientLoginData *list = clientList;
	clientLoginData *prev = NULL;
	clientLoginData *next = NULL;

	for( ; list != NULL; list = list->next)
	{
		if(list->index == ind)
		{
			if(prev != NULL)
			{
				prev->next = list->next;
			}

			break;
		}

		prev = list;
	}

	if(list == clientList)
	{
		if(list)
		{
			next = list->next;
			free(list);
		}

		list = NULL;
		clientList = next;
	}
	else
	{
		if(list)
		{
			next = list->next;
			free(list);
		}

		list = next;
	}

	RefreshPlayerList();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveClients(void)
{
	clientLoginData *list = clientList;
	clientLoginData *next;

	while(list != NULL)
	{
		if(list)
		{
			next = list->next;
			free(list);
		}

		list = next;
	}

	clientList = NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::AddGame(char *name, int ind, bool inProgress)
{
	// First get a pointer to the beginning of client list
	CArmyWar *list = gameList;
	CArmyWar *prev;

	// No clients yet, adding the first one
	if(gameList == NULL)
	{
		gameList = new CArmyWar;

		gameList->SetName(name);
		gameList->SetGameIndex(ind);
		gameList->SetInProgress(inProgress);
		gameList->next = NULL;

		// If game's host index number equals our's, connect
		if(ind == localClient->index)
		{
			LogString("Joining game %d (we are host)", gameAmount);

			gameList->StartConnection(gameAmount);
			localGame = gameList;
		}
	}
	else
	{
		prev = list;
		list = gameList->next;

		while(list != NULL)
		{
			prev = list;
			list = list->next;
		}

		list = new CArmyWar;

		list->SetName(name);
		list->SetGameIndex(ind);
		list->SetInProgress(inProgress);
		list->next = NULL;

		// If game's host index number equals our's, connect
		if(ind == localClient->index)
		{
			LogString("Joining game %d (we are host)", gameAmount);

			list->StartConnection(gameAmount);
			localGame = list;
		}

		prev->next = list;
	}

	gameAmount++;

	RefreshGameList();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveGame(char *name)
{
	CArmyWar *list = gameList;
	CArmyWar *prev = NULL;
	CArmyWar *next = NULL;

	for( ; list != NULL; list = list->next)
	{
		if(strcmp(name, list->GetName()) == 0)
		{
			if(prev != NULL)
			{
				prev->next = list->next;
			}

			break;
		}

		prev = list;
	}

	// If our game is removed, update local game pointer
	if(list == Lobby.GetLocalGame())
	{
		DestroyWindow(hWnd_JoinGameDialog);

		Lobby.SetLocalGame(NULL);
	}

	if(list == gameList)
	{
		if(list)
		{
			next = list->next;
			delete list;
		}

		list = NULL;
		gameList = next;
	}
	else
	{
		if(list)
		{
			next = list->next;
			delete list;
		}

		list = next;
	}

	gameAmount--;

	RefreshGameList();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveGames(void)
{
	CArmyWar *list = gameList;
	CArmyWar *next;

	while(list != NULL)
	{
		if(list)
		{
			next = list->next;
			delete list;
		}

		list = next;
	}

	gameList = NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CArmyWar *CLobby::GetGamePointer(int ind)
{
	for(CArmyWar *gList = gameList; gList != NULL; gList = gList->next)
	{
		if(gList->GetGameIndex() == ind)
			return gList;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RequestGameData(void)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_GAMEDATA);
	message.AddSequences(networkClient);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::SendChat(char *text)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_CHAT);
	message.AddSequences(networkClient);
	message.WriteString(text);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::SendCreateGame(char *gamename)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_CREATEGAME);
	message.AddSequences(networkClient);
	message.WriteString(gamename);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::SendRemoveGame(char *gamename)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_REMOVEGAME);
	message.AddSequences(networkClient);
	message.WriteString(gamename);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::SendStartGame(int ind)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_STARTGAME);
	message.AddSequences(networkClient);
	message.WriteByte(ind);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::SendKeepAlive(void)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_KEEPALIVE);
	message.AddSequences(networkClient);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::Connect(char *name, char *password)
{
	LogString("CLobby::Connect");

	timeConnecting = 0;
	SetWindowText(hWnd_Application, "ARMY WAR Online 2.0 - connecting ...");

	networkClient->SendConnect(name);

	dreamMessage message;
	char data[1400];

	message.Init(data, sizeof(data));
	message.WriteByte(USER_MES_LOGIN);						// type
	message.AddSequences(networkClient);					// sequences
	message.WriteString(name);								// name
	message.WriteString(password);							// password

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::Disconnect(void)
{
	LogString("CLobby::Disconnect");

	timeConnecting = -1;
	SetWindowText(hWnd_Application, "ARMY WAR Online 2.0");

	localClient = NULL;
	localGame	= NULL;

	RemoveClients();
	RemoveGames();
	networkClient->SendDisconnect();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RunNetwork(int msec)
{
	if(networkClient->GetConnectionState() == DREAMSOCK_DISCONNECTED)
		return;

	static int time = 0;
	static int keepalive = 0;
	time += msec;
	keepalive += msec;

	if(keepalive > 20000)
	{
		SendKeepAlive();
		keepalive = 0;
	}

	// If timeconnecting is negative, we are connected or connection failed
	if(timeConnecting > -1)
		timeConnecting += msec;

	if(timeConnecting > 3000)
	{
		SetWindowText(hWnd_Application, "ARMY WAR Online 2.0");

		MessageBox(NULL, "Could not connect", "Connection error", MB_OK);
		timeConnecting = -1;

		ShowWindow(hWnd_LoginDialog, SW_SHOW);
	}

	// framerate is too high
	if(time < (1000 / 30))
		return;

	time = 0;

	ReadPackets();
}
