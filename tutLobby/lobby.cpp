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
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RefreshJoinedPlayersList(void)
{
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
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveGame(char *name)
{
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobby::RemoveGames(void)
{
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
	message.WriteByte(USER_MES_LOGIN);			// type
	message.AddSequences(networkClient);		// sequences
	message.WriteString(name);					// name
	message.WriteString(password);				// password

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

	// If timeConnecting is negative, we are connected or connection failed
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
