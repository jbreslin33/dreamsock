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
void CArmyWar::StartConnection(int ind)
{
	LogString("StartConnection %d", ind);

	gameIndex = ind;

	int ret = networkClient->Initialise("", serverIP, 30004 + gameIndex);

	if(ret == DREAMSOCK_CLIENT_ERROR)
	{
		char text[64];
		sprintf(text, "Could not open client socket");

		MessageBox(NULL, text, "Error", MB_OK);
	}

	Connect();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::ReadPackets(void)
{
	char data[1400];
	struct sockaddr address;

	clientData *clList;

	int type;
	int ind;
	int local;
	int ret;

	char name[50];

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

			if(clientList == NULL)
			{
				LogString("clientList == NULL, sending remove game %s", gamename);
				Lobby.SendRemoveGame(gamename);
			}
			break;

		case USER_MES_FRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			for(clList = clientList; clList != NULL; clList = clList->next)
			{
//				LogString("Reading DELTAFRAME for client %d", clList->index);
				ReadDeltaMoveCommand(&mes, clList);
			}

			break;

		case USER_MES_NONDELTAFRAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			clList = clientList;

			for(clList = clientList; clList != NULL; clList = clList->next)
			{
				LogString("Reading NONDELTAFRAME for client %d", clList->index);
				ReadMoveCommand(&mes, clList);
			}

			break;

		case USER_MES_SERVEREXIT:
			MessageBox(NULL, "Server disconnected", "Info", MB_OK);
			Disconnect();
			break;

		case USER_MES_STARTGAME:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			DestroyWindow(hWnd_JoinGameDialog);

			InitialiseEngine();

			break;

		case USER_MES_MAPDATA:
			// Skip sequences
			mes.ReadShort();
			mes.ReadShort();

			for(int m = 0; m < 300; m++)
			{
				int i = mes.ReadByte();
				int j = mes.ReadByte();

				mapdata[i][j] = true;
			}

			break;

		}
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::AddClient(int local, int ind, char *name)
{
	// First get a pointer to the beginning of client list
	clientData *list = clientList;
	clientData *prev;

	LogString("App: Client: Adding client with index %d", ind);

	// No clients yet, adding the first one
	if(clientList == NULL)
	{
		LogString("App: Client: Adding first client");

		clientList = (clientData *) calloc(1, sizeof(clientData));

		if(local)
		{
			LogString("App: Client: This one is local");
			localClient = clientList;
		}

		clientList->index = ind;
		strcpy(clientList->nickname, name);

		if(clients % 2 == 0)
			clientList->team = RED_TEAM;
		else
			clientList->team = BLUE_TEAM;

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

		list = (clientData *) calloc(1, sizeof(clientData));

		if(local)
		{
			LogString("App: Client: This one is local");
			localClient = list;
		}

		list->index = ind;
		strcpy(list->nickname, name);

		if(clients % 2 == 0)
			list->team = RED_TEAM;
		else
			list->team = BLUE_TEAM;

		list->next = NULL;
		prev->next = list;
	}

	clients++;

	// If we just joined the game, request a non-delta compressed frame
	if(local)
		SendRequestNonDeltaFrame();

	Lobby.RefreshJoinedPlayersList();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::RemoveClient(int ind)
{
	clientData *list = clientList;
	clientData *prev = NULL;
	clientData *next = NULL;

	// Look for correct client and update list
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

	// First entry
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

	// Other
	else
	{
		if(list)
		{
			next = list->next;
			free(list);
		}

		list = next;
	}

	clients--;

	Lobby.RefreshJoinedPlayersList();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::RemoveClients(void)
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
	clients = 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::SendCommand(void)
{
	if(networkClient->GetConnectionState() != DREAMSOCK_CONNECTED)
		return;

	dreamMessage message;
	char data[1400];

	int i = networkClient->GetOutgoingSequence() & (COMMAND_HISTORY_SIZE-1);

	message.Init(data, sizeof(data));
	message.WriteByte(USER_MES_FRAME);						// type
	message.AddSequences(networkClient);					// sequences

	// Build delta-compressed move command
	BuildDeltaMoveCommand(&message, &inputClient);

	// Send the packet
	networkClient->SendPacket(&message);

	// Store the command to the input client's history
	memcpy(&inputClient.frame[i], &inputClient.command, sizeof(command_t));

	clientData *clList = clientList;

	// Store the commands to the clients' history
	for( ; clList != NULL; clList = clList->next)
	{
		memcpy(&clList->frame[i], &clList->command, sizeof(command_t));
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::SendStartGame(void)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_STARTGAME);
	message.AddSequences(networkClient);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::SendRequestNonDeltaFrame(void)
{
	char data[1400];
	dreamMessage message;
	message.Init(data, sizeof(data));

	message.WriteByte(USER_MES_NONDELTAFRAME);
	message.AddSequences(networkClient);

	networkClient->SendPacket(&message);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::Connect(void)
{
	if(init)
	{
		LogString("ArmyWar already initialised");
		return;
	}

	LogString("CArmyWar::Connect");

	init = true;

	networkClient->SendConnect(Lobby.GetLocalClient()->nickname);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::Disconnect(void)
{
	if(!init)
		return;

	LogString("CArmyWar::Disconnect");

	init = false;
	localClient = NULL;
	memset(&inputClient, 0, sizeof(clientData));

	networkClient->SendDisconnect();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::ReadMoveCommand(dreamMessage *mes, clientData *client)
{
	// Key
	client->serverFrame.key				= mes->ReadByte();

	// Heading
	client->serverFrame.heading			= mes->ReadShort();

	// Origin
	client->serverFrame.origin.x		= mes->ReadFloat();
	client->serverFrame.origin.y		= mes->ReadFloat();
	client->serverFrame.vel.x			= mes->ReadFloat();
	client->serverFrame.vel.y			= mes->ReadFloat();

	client->serverFrame.bullet.origin.x	= mes->ReadFloat();
	client->serverFrame.bullet.origin.y	= mes->ReadFloat();
	client->serverFrame.bullet.vel.x	= mes->ReadFloat();
	client->serverFrame.bullet.vel.y	= mes->ReadFloat();
	client->serverFrame.bullet.lifetime	= mes->ReadShort();
	client->serverFrame.bullet.shot		= mes->ReadByte();

	int playerWithFlagIndex = mes->ReadShort();

	if(playerWithFlagIndex != -1)
	{
		playerWithFlag = GetClientPointer(playerWithFlagIndex);
	}

	flagX = mes->ReadFloat();
	flagY = mes->ReadFloat();

	redScore = mes->ReadByte();
	blueScore = mes->ReadByte();

	// Read time to run command
	client->serverFrame.msec = mes->ReadByte();

	memcpy(&client->command, &client->serverFrame, sizeof(command_t));

	// Fill the history array with the position we got
	for(int f = 0; f < COMMAND_HISTORY_SIZE; f++)
	{
		client->frame[f].predictedOrigin.x = client->command.origin.x;
		client->frame[f].predictedOrigin.y = client->command.origin.y;
		client->frame[f].bullet.predictedOrigin.x = client->command.bullet.origin.x;
		client->frame[f].bullet.predictedOrigin.y = client->command.bullet.origin.y;
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::ReadDeltaMoveCommand(dreamMessage *mes, clientData *client)
{
	int processedFrame;
	int flags = 0;

	// Flags
	flags = mes->ReadByte();

	// Key
	if(flags & CMD_KEY)
	{
		client->serverFrame.key = mes->ReadByte();

		client->command.key = client->serverFrame.key;
		LogString("Client %d: Read key %d", client->index, client->command.key);
	}

	if(flags & CMD_ORIGIN || flags & CMD_BULLET)
	{
		processedFrame = mes->ReadByte();
	}

	// Origin
	if(flags & CMD_ORIGIN)
	{
		client->serverFrame.origin.x = mes->ReadFloat();
		client->serverFrame.origin.y = mes->ReadFloat();

		client->serverFrame.vel.x = mes->ReadFloat();
		client->serverFrame.vel.y = mes->ReadFloat();

		if(client == localClient)
		{
			CheckPredictionError(processedFrame);
		}
		else
		{
			client->command.origin.x = client->serverFrame.origin.x;
			client->command.origin.y = client->serverFrame.origin.y;
		}
	}

	if(flags & CMD_BULLET)
	{
		client->serverFrame.bullet.origin.x = mes->ReadFloat();
		client->serverFrame.bullet.origin.y = mes->ReadFloat();
		client->serverFrame.bullet.vel.x = mes->ReadFloat();
		client->serverFrame.bullet.vel.y = mes->ReadFloat();
		client->serverFrame.bullet.shot = mes->ReadByte();

		client->command.bullet.shot = client->serverFrame.bullet.shot;

		if(client == localClient)
		{
			CheckBulletPredictionError(processedFrame);
		}
		else
		{
			client->command.bullet.origin.x = client->serverFrame.bullet.origin.x;
			client->command.bullet.origin.y = client->serverFrame.bullet.origin.y;
		}
	}

	// Flag & points
	if(flags & CMD_FLAG)
	{
		int playerWithFlagIndex = mes->ReadShort();

		if(playerWithFlagIndex != 0)
		{
			LogString("FLAG playerWithFlagIndex %d", playerWithFlagIndex);
			playerWithFlag = GetClientPointer(playerWithFlagIndex);
		}
		else
		{
			playerWithFlag = NULL;
		}

		flagX = mes->ReadFloat();
		flagY = mes->ReadFloat();

		redScore = mes->ReadByte();
		blueScore = mes->ReadByte();

		CheckVictory();
	}

	// Someone died
	if(flags & CMD_KILL)
	{
		int died = mes->ReadByte();

		if(died)
			KillPlayer(client->index);
	}

	// Read time to run command
	client->command.msec = mes->ReadByte();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::BuildDeltaMoveCommand(dreamMessage *mes, clientData *theClient)
{
	int flags = 0;
	int last = (networkClient->GetOutgoingSequence() - 1) & (COMMAND_HISTORY_SIZE-1);

	// Check what needs to be updated
	if(theClient->frame[last].key != theClient->command.key)
		flags |= CMD_KEY;

	// Add to the message
	// Flags
	mes->WriteByte(flags);

	// Key
	if(flags & CMD_KEY)
	{
		mes->WriteByte(theClient->command.key);
	}

	mes->WriteByte(theClient->command.msec);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::RunNetwork(int msec)
{
	static int time = 0;
	time += msec;

	// Framerate is too high
	if(time < (1000 / 60))
		return;

	frametime = time / 1000.0f;
	time = 0;

	// Read packets from server, and send new commands
	ReadPackets();
	SendCommand();

	int ack = networkClient->GetIncomingAcknowledged();
	int current = networkClient->GetOutgoingSequence();

	// Check that we haven't gone too far
	if(current - ack > COMMAND_HISTORY_SIZE)
		return;

	// Predict the frames that we are waiting from the server
	for(int a = ack + 1; a < current; a++)
	{
		int prevframe = (a-1) & (COMMAND_HISTORY_SIZE-1);
		int frame = a & (COMMAND_HISTORY_SIZE-1);

		PredictMovement(prevframe, frame);
	}

	MoveObjects();
}
