/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game client                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CSignin::CSignin()
{
	networkClient = new dreamClient;
	clientList	= NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CSignin::~CSignin()
{
	delete networkClient;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::ReadPackets(void)
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

			RemoveClient(ind);
			break;

		case USER_MES_SERVEREXIT:
			MessageBox(NULL, "Server disconnected", "Info", MB_OK);
			Disconnect();
			break;

		case USER_MES_SIGNIN:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			ret = mes.ReadShort();

			LogString("Got lobby signin respond %d", ret);

			if(ret != SIGNIN_RESULT_ACCEPTED)
			{
				MessageBox(hWnd_LoginDialog,
					"Sign in did not succeed. Try again.", "Error", MB_OK);
			}
			else
			{
				MessageBox(hWnd_LoginDialog,
					"Sign in successfull. You can now login.", "Info", MB_OK);
			}

			Disconnect();
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::AddClient(int local, int ind, char *name)
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
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::RemoveClient(int ind)
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
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::RemoveClients(void)
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
void CSignin::SendSignIn(char *nickname, char *firstname,
						char *surname, int age, char *gender, char *password, char *email)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_SIGNIN);
	message.AddSequences(networkClient);
	message.WriteString(nickname);
	message.WriteString(firstname);
	message.WriteString(surname);
	message.WriteByte(age);
	message.WriteString(gender);
	message.WriteString(password);
	message.WriteString(email);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::SendKeepAlive(void)
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
void CSignin::Connect(char *name)
{
	LogString("CSignin::Connect");

	networkClient->SendConnect(name);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::Disconnect(void)
{
	LogString("CSignin::Disconnect");

	localClient = NULL;

	RemoveClients();
	networkClient->SendDisconnect();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CSignin::RunNetwork(int msec)
{
	if(networkClient->GetConnectionState() == DREAMSOCK_DISCONNECTED)
		return;

	static int keepalive = 0;
	keepalive += msec;

	if(keepalive > 20000)
	{
		SendKeepAlive();
		keepalive = 0;
	}

	ReadPackets();
}
