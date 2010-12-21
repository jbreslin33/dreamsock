/******************************************/
/* Developer's Guide to Multiplayer Games */
/* Tutorial game server                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include <malloc.h>
#include <mysql++>

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CLobbyServer::CLobbyServer()
{
	networkServer = new dreamServer;

	clientList	= NULL;
	gameList	= NULL;

	gameAmount	= 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CLobbyServer::~CLobbyServer()
{
	delete networkServer;
}

//-----------------------------------------------------------------------------
// Name: InitNetwork()
// Desc: Initialize network
//-----------------------------------------------------------------------------
int CLobbyServer::InitNetwork(void)
{
	// Initialize dreamSock and the server
	if(dreamSock_Initialize() != 0)
	{
		LogString("Error initialising Communication Library!");
		return 1;
	}

	int ret = networkServer->Initialise("", 30003);

	if(ret == DREAMSOCK_SERVER_ERROR)
	{
#ifdef WIN32
		char text[64];
		sprintf(text, "Could not open server on port %d", networkServer->GetPort());

		MessageBox(NULL, text, "Error", MB_OK);
#else
		LogString("Could not open server on port %d", networkServer->GetPort());
#endif
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: ShutdownNetwork()
// Desc: Shutdown network
//-----------------------------------------------------------------------------
void CLobbyServer::ShutdownNetwork(void)
{
	LogString("Shutting down lobby server...");

	SendExitNotification();
	RemoveClients();
	RemoveGames();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::ReadPackets(void)
{
	char data[1400];

	int type;
	int ret;

	// Some incoming data
	char name[30];
	char password[50];
	char chatter[50];
	int respond;
	int ind;

	struct sockaddr address;

	clientData *clList;
	CArmyWarServer *gList;

	dreamMessage mes;
	mes.Init(data, sizeof(data));

	// Get the packet from the socket
	try
	{
		while(ret = networkServer->GetPacket(mes.data, &address))
		{
			mes.SetSize(ret);
			mes.BeginReading();

			type = mes.ReadByte();

			// Check the type of the message
			switch(type)
			{
			case DREAMSOCK_MES_CONNECT:
				AddClient();
				break;

			case DREAMSOCK_MES_DISCONNECT:
				RemoveClient(&address);

				if(clientList == NULL)
					RemoveGames();

				gList = gameList;

				for( ; gList != NULL; gList = gList->next)
				{
					if(gList->GetClientList() == NULL)
					{
						RemoveGame(gList->GetName());
					}
				}

				break;

			case USER_MES_LOGIN:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				strcpy(name, mes.ReadString());
				strcpy(password, mes.ReadString());

				LogString("Lobby: Player %s logged in", name);
/*
				try 
				{
					// -> Create a connection to the database
					Connection con("onlinedata", "127.0.0.1");
				
					// -> Create a query object that is bound to our connection
					Query query = con.query();
				
					// -> Assign the query to that object
					query << "SELECT id,firstname,password FROM playerdata WHERE nickname = \"" << name << "\"";
				
					// -> Store the results from then query
					Result res = query.store();
				
					Result::iterator i;
					Row row;
					i = res.begin();
				
					if(i!=res.end())
					{
						row = *i;
						if(!strcmp(password, row["password"]))
						{
							// -> Update the 'lastlogin' field to current date and time
							query << "UPDATE playerdata SET lastlogin = NULL WHERE id = " << row["id"];
							query.execute();
						
							// -> Set the player to 'online'
							query << "UPDATE playerdata SET online = 1 WHERE id = " << row["id"];
							query.execute();
						
							// -> Player Login Successful!
							respond = LOBBYLOGIN_RESULT_ACCEPTED;
						}
						else
						{
							// -> Password did not match
							respond = LOBBYLOGIN_RESULT_PASSWORDBAD;
						}
					}
					else
					{
						// -> Nickname could not be found
						respond = LOBBYLOGIN_RESULT_USERNAMEBAD;
					}
				}
				catch (BadQuery er) // handle any connection errors
				{ 
					// -> mySQL Server not running?
					respond = LOBBYLOGIN_RESULT_MYSQLERROR;
				}
*/
				// TEMP FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				respond = LOBBYLOGIN_RESULT_ACCEPTED;
				// TEMP FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

				// Find the correct client by comparing addresses
				clList = clientList;

				for( ; clList != NULL; clList = clList->next)
				{
					if(memcmp(&clList->address, &address, sizeof(address)) == 0)
					{
						clList->netClient->message.Init(clList->netClient->message.outgoingData,
							sizeof(clList->netClient->message.outgoingData));

						clList->netClient->message.WriteByte(USER_MES_LOGIN);			// type
						clList->netClient->message.AddSequences(clList->netClient);		// sequences
						clList->netClient->message.WriteShort(respond);					// respond
						clList->netClient->SendPacket();

						LogString("Sending lobby login respond");

						break;
					}
				}
				break;

			case USER_MES_CHAT:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				// Read chat text
				strcpy(chatter, mes.ReadString());

				// Send the chat text to everybody
				for(clList = clientList; clList != NULL; clList = clList->next)
				{
					clList->netClient->message.Init(clList->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					clList->netClient->message.WriteByte(USER_MES_CHAT);			// type
					clList->netClient->message.AddSequences(clList->netClient);		// sequences
					clList->netClient->message.WriteString(chatter);				// text
				}

				networkServer->SendPackets();

				break;

			case USER_MES_CREATEGAME:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				// Read game name
				strcpy(name, mes.ReadString());

				AddGame(name);

				// Find the correct client by comparing addresses
				clList = clientList;

				for( ; clList != NULL; clList = clList->next)
				{
					if(memcmp(&clList->address, &address, sizeof(address)) == 0)
					{
						ind = clList->netClient->GetIndex();
						break;
					}
				}

				// Send to everybody
				for(clList = clientList; clList != NULL; clList = clList->next)
				{
					clList->netClient->message.Init(clList->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					clList->netClient->message.WriteByte(USER_MES_CREATEGAME);		// type
					clList->netClient->message.AddSequences(clList->netClient);		// sequences
					clList->netClient->message.WriteString(name);					// game name
					clList->netClient->message.WriteShort(ind);						// host's index
					clList->netClient->message.WriteByte(0);						// in progress?
				}

				networkServer->SendPackets();

				break;

			case USER_MES_REMOVEGAME:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				// Read game name
				strcpy(name, mes.ReadString());

				LogString("REMOVING %s", name);

				RemoveGame(name);

				// Send to everybody
				for(clList = clientList; clList != NULL; clList = clList->next)
				{
					clList->netClient->message.Init(clList->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					clList->netClient->message.WriteByte(USER_MES_REMOVEGAME);		// type
					clList->netClient->message.AddSequences(clList->netClient);		// sequences
					clList->netClient->message.WriteString(name);					// game name
				}

				networkServer->SendPackets();

				break;

			case USER_MES_STARTGAME:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				ind = mes.ReadByte();

				GetGamePointer(ind)->SetInProgress(true);

				// Send to everybody
				for(clList = clientList; clList != NULL; clList = clList->next)
				{
					clList->netClient->message.Init(clList->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					clList->netClient->message.WriteByte(USER_MES_STARTGAME);		// type
					clList->netClient->message.AddSequences(clList->netClient);		// sequences
					clList->netClient->message.WriteShort(ind);						// game index
				}

				networkServer->SendPackets();

				break;

			case USER_MES_GAMEDATA:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				// Find the correct client by comparing addresses
				clList = clientList;

				for( ; clList != NULL; clList = clList->next)
				{
					if(memcmp(&clList->address, &address, sizeof(address)) == 0)
					{
						gList = gameList;

						for( ; gList != NULL; gList = gList->next)
						{
							clList->netClient->message.Init(clList->netClient->message.outgoingData,
								sizeof(clList->netClient->message.outgoingData));

							clList->netClient->message.WriteByte(USER_MES_CREATEGAME);		// type
							clList->netClient->message.AddSequences(clList->netClient);		// sequences
							clList->netClient->message.WriteString(gList->GetName());		// game name
							clList->netClient->message.WriteShort(-1);						// host's index
							clList->netClient->message.WriteByte((int) gList->GetInProgress());	// in progress?
							clList->netClient->SendPacket();
						}

						break;
					}
				}

				break;
			}
		}
	}
	catch(...)
	{
		LogString("Unknown Exception caught in Lobby ReadPackets loop");

#ifdef WIN32
		MessageBox(NULL, "Unknown Exception caught in Lobby ReadPackets loop", "Error", MB_OK | MB_TASKMODAL);
#endif
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::SendExitNotification(void)
{
	clientData *toClient = clientList;

	for( ; toClient != NULL; toClient = toClient->next)
	{
		toClient->netClient->message.Init(toClient->netClient->message.outgoingData,
			sizeof(toClient->netClient->message.outgoingData));

		toClient->netClient->message.WriteByte(USER_MES_SERVEREXIT);	// type
		toClient->netClient->message.AddSequences(toClient->netClient);	// sequences
	}

	networkServer->SendPackets();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::AddClient(void)
{
	// First get a pointer to the beginning of client list
	clientData *list = clientList;
	clientData *prev;
	dreamClient *netList = networkServer->GetClientList();

	// No clients yet, adding the first one
	if(clientList == NULL)
	{
		LogString("App: Server: Adding first client");

		clientList = (clientData *) calloc(1, sizeof(clientData));

		clientList->netClient = netList;

//		memcpy(&clientList->address,
//			&clientList->netClient->myaddress, sizeof(struct sockaddr));
		memcpy(&clientList->address,
			clientList->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		clientList->next = NULL;
	}
	else
	{
		LogString("App: Server: Adding another client");

		prev = list;
		list = clientList->next;
		netList = netList->next;

		while(list != NULL)
		{
			prev = list;
			list = list->next;
			netList = netList->next;
		}

		list = (clientData *) calloc(1, sizeof(clientData));

		list->netClient = netList;

//		memcpy(&list->address,
//			&list->netClient->myaddress, sizeof(struct sockaddr));
		memcpy(&list->address,
			list->netClient->GetSocketAddress(), sizeof(struct sockaddr));

		list->next = NULL;

		prev->next = list;
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::RemoveClient(struct sockaddr *address)
{
	clientData *list = clientList;
	clientData *prev = NULL;
	clientData *next = NULL;

	for( ; list != NULL; list = list->next)
	{
		if(memcmp(&list->address, address, sizeof(address)) == 0)
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
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::RemoveClients(void)
{
	clientData *list = clientList;
	clientData *next;

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
void CLobbyServer::AddGame(char *name)
{
	// First get a pointer to the beginning of client list
	CArmyWarServer *list = gameList;
	CArmyWarServer *prev;

	// No clients yet, adding the first one
	if(gameList == NULL)
	{
		gameList = new CArmyWarServer;

		gameList->SetName(name);
		gameList->next = NULL;
		gameList->SetIndex(gameAmount);
		gameList->GenerateRandomMap();

		if(gameList->InitNetwork(GetGameAmount()) != 0)
		{
			LogString("Could not create game server");
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

		list = new CArmyWarServer;

		list->SetName(name);
		list->next = NULL;
		list->SetIndex(gameAmount);
		list->GenerateRandomMap();

		if(list->InitNetwork(GetGameAmount()) != 0)
		{
			LogString("Could not create game server");
		}

		prev->next = list;
	}

	gameAmount++;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::RemoveGame(char *name)
{
	CArmyWarServer *list = gameList;
	CArmyWarServer *prev = NULL;
	CArmyWarServer *next = NULL;

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

	if(list == gameList)
	{
		if(list)
		{
			list->ShutdownNetwork();

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
			list->ShutdownNetwork();

			next = list->next;
			delete list;
		}

		list = next;
	}

	gameAmount--;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::RemoveGames(void)
{
	CArmyWarServer *list = gameList;
	CArmyWarServer *next;

	while(list != NULL)
	{
		if(list)
		{
			list->ShutdownNetwork();

			next = list->next;
			delete list;
		}

		list = next;
	}

	gameList = NULL;
	gameAmount = 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CArmyWarServer *CLobbyServer::GetGamePointer(int ind)
{
	for(CArmyWarServer *gList = gameList; gList != NULL; gList = gList->next)
	{
		if(gList->GetIndex() == ind)
			return gList;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CLobbyServer::Frame(int msec)
{
	static int time = 0;
	time += msec;

	if(time > 5000)
	{
		CArmyWarServer *gList = gameList;

		for( ; gList != NULL; gList = gList->next)
		{
			if(gList->GetClientList() == NULL)
			{
				// Send to everybody
				for(clientData *clList = clientList; clList != NULL; clList = clList->next)
				{
					clList->netClient->message.Init(clList->netClient->message.outgoingData,
						sizeof(clList->netClient->message.outgoingData));

					clList->netClient->message.WriteByte(USER_MES_REMOVEGAME);		// type
					clList->netClient->message.AddSequences(clList->netClient);		// sequences
					clList->netClient->message.WriteString(gList->GetName());		// game name
				}

				networkServer->SendPackets();

				RemoveGame(gList->GetName());
			}
		}

		time = 0;
	}

	ReadPackets();
}
