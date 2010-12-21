/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game server                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include <malloc.h>

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CSigninServer::CSigninServer()
{
	networkServer = new dreamServer;
	clientList	= NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CSigninServer::~CSigninServer()
{
	delete networkServer;
}

//-----------------------------------------------------------------------------
// Name: InitNetwork()
// Desc: Initialize network
//-----------------------------------------------------------------------------
int CSigninServer::InitNetwork(void)
{
	// Initialize dreamSock and the server
	if(dreamSock_Initialize() != 0)
	{
		LogString("Error initialising Communication Library!");
		return 1;
	}

	int ret = networkServer->Initialise("", 30002);

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
void CSigninServer::ShutdownNetwork(void)
{
	LogString("Shutting down sign-in server...");

	SendExitNotification();
	RemoveClients();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSigninServer::ReadPackets(void)
{
	char data[1400];

	int type;
	int ret;

	// Some incoming data
	char password[50];
	int respond;

	char nickname[30];
	char surname[30];
	char firstname[30];
	char gender[10];
	char email[30];
	int age;

	struct sockaddr address;

	clientLoginData *clList;

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
				break;

			case USER_MES_SIGNIN:
				// Skip sequences
				mes.ReadShort();
				mes.ReadShort();

				strcpy(nickname, mes.ReadString());
				strcpy(firstname, mes.ReadString());
				strcpy(surname, mes.ReadString());
				age = mes.ReadByte();
				strcpy(gender, mes.ReadString());
				strcpy(password, mes.ReadString());
				strcpy(email, mes.ReadString());

				LogString("Signin: Player %s signed in", nickname);

				// mySQL Connection comes here

				respond = SIGNIN_RESULT_ACCEPTED;
			
				// Find the correct client by comparing addresses
				clList = clientList;

				for( ; clList != NULL; clList = clList->next)
				{
					if(memcmp(&clList->address, &address, sizeof(address)) == 0)
					{
						clList->netClient->message.Init(clList->netClient->message.outgoingData,
							sizeof(clList->netClient->message.outgoingData));

						clList->netClient->message.WriteByte(USER_MES_SIGNIN);			// type
						clList->netClient->message.AddSequences(clList->netClient);		// sequences
						clList->netClient->message.WriteShort(respond);					// respond
						clList->netClient->SendPacket();

						LogString("Sending signin respond");

						break;
					}
				}

				break;
			}
		}
	}
	catch(...)
	{
		LogString("Unknown Exception caught in Signin ReadPackets loop");

#ifdef WIN32
		MessageBox(NULL, "Unknown Exception caught in Signin ReadPackets loop", "Error", MB_OK | MB_TASKMODAL);
#endif
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSigninServer::SendExitNotification(void)
{
	clientLoginData *toClient = clientList;

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
void CSigninServer::AddClient(void)
{
	// First get a pointer to the beginning of client list
	clientLoginData *list = clientList;
	clientLoginData *prev;
	dreamClient *netList = networkServer->GetClientList();

	// No clients yet, adding the first one
	if(clientList == NULL)
	{
		LogString("App: Server: Adding first client");

		clientList = (clientLoginData *) calloc(1, sizeof(clientLoginData));

		clientList->netClient = netList;

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

		list = (clientLoginData *) calloc(1, sizeof(clientLoginData));

		list->netClient = netList;

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
void CSigninServer::RemoveClient(struct sockaddr *address)
{
	clientLoginData *list = clientList;
	clientLoginData *prev = NULL;
	clientLoginData *next = NULL;

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
void CSigninServer::RemoveClients(void)
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
void CSigninServer::Frame(int msec)
{
	ReadPackets();
}
