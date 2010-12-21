/******************************************/
/* MMOG programmer's guide                */
/* Tutorial game server                   */
/* Programming:						      */
/* Teijo Hakala						      */
/******************************************/

#include "common.h"
#include <fstream>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
float VectorLength(VECTOR2D *vec)
{
	return (float) sqrt(vec->x*vec->x + vec->y*vec->y);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
VECTOR2D VectorSubstract(VECTOR2D *vec1, VECTOR2D *vec2)
{
	VECTOR2D vec;

	vec.x = vec1->x - vec2->x;
	vec.y = vec1->y - vec2->y;

	return vec;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CArmyWarServer::CArmyWarServer()
{
	networkServer = new dreamServer;

	clientList	= NULL;
	clients		= 0;

	realtime	= 0;
	servertime	= 0;

	inProgress  = false;

	index		= 0;
	next		= NULL;

	flagX		= 0.0f;
	flagY		= 0.0f;
	playerWithFlag = NULL;
	updateFlag	= false;

	redScore	= 0;
	blueScore	= 0;

	framenum	= 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CArmyWarServer::~CArmyWarServer()
{
	delete networkServer;
}

//-----------------------------------------------------------------------------
// Name: InitNetwork()
// Desc: Initialize network
//-----------------------------------------------------------------------------
int CArmyWarServer::InitNetwork(int gameAmount)
{
	if(dreamSock_Initialize() != 0)
	{
		LogString("Error initialising Communication Library!");
		return 1;
	}

	LogString("Initialising game %d", gameAmount);

	// Create the game servers on new ports, starting from 30004
	int ret = networkServer->Initialise("", 30004 + gameAmount);

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
void CArmyWarServer::ShutdownNetwork(void)
{
	LogString("Shutting down game server...");

	RemoveClients();

	networkServer->Uninitialise();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::GenerateRandomMap(void)
{
	// Make all land passable
	for(int i = 0; i < 100; i++)
	{
		for(int j = 0; j < 100; j++)
		{
			mapdata[i][j] = false;
		}
	}

	// Use the game's index number for random seed
	srand(index);

	// Place some random trees (avoiding the players start locations)
	for(int m = 0; m < 300; m++)
	{
		mapdata[rand()%100][(rand()%80)+10] = true;
	}

	// Set the flag position
	flagX = 49*32;
	flagY = 49*32;

	playerWithFlag = NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::CalculateVelocity(command_t *command, float frametime)
{
	int checkX;
	int checkY;

	float multiplier = 100.0f;

	command->vel.x = 0.0f;
	command->vel.y = 0.0f;

	if(command->key & KEY_UP)
	{
		checkX = (int) (command->origin.x / 32.0f);
		checkY = (int) ((command->origin.y - multiplier * frametime) / 32.0f);

		if(mapdata[checkX][checkY] == false)
			command->vel.y += -multiplier * frametime;
	}

	if(command->key & KEY_DOWN)
	{
		checkX = (int) (command->origin.x / 32.0f);
		checkY = (int) ((command->origin.y + multiplier * frametime) / 32.0f);

		if(mapdata[checkX][checkY] == false)
			command->vel.y += multiplier * frametime;
	}

	if(command->key & KEY_LEFT)
	{
		checkX = (int) ((command->origin.x - multiplier * frametime) / 32.0f);
		checkY = (int) (command->origin.y / 32.0f);

		if(mapdata[checkX][checkY] == false)
			command->vel.x += -multiplier * frametime;
	}

	if(command->key & KEY_RIGHT)
	{
		checkX = (int) ((command->origin.x + multiplier * frametime) / 32.0f);
		checkY = (int) (command->origin.y / 32.0f);

		if(mapdata[checkX][checkY] == false)
			command->vel.x += multiplier * frametime;
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::CalculateHeading(command_t *command)
{
	// Left
	if( (command->vel.x > 0.0f) &&
		(command->vel.y == 0.0f) )
	{
		command->heading = EAST;
	}

	// Right
	if( (command->vel.x < 0.0f) &&
		(command->vel.y == 0.0f) )
	{
		command->heading = WEST;
	}

	// Down
	if( (command->vel.y > 0.0f) &&
		(command->vel.x == 0.0f) )
	{
		command->heading = SOUTH;
	}

	// Up
	if( (command->vel.y < 0.0f) &&
		(command->vel.x == 0.0f) )
	{
		command->heading = NORTH;
	}

	// Down-Left
	if( (command->vel.x > 0.0f) &&
		(command->vel.y > 0.0f) )
	{
		command->heading = SOUTHEAST;
	}

	// Up-Left
	if( (command->vel.x > 0.0f) &&
		(command->vel.y < 0.0f) )
	{
		command->heading = NORTHEAST;
	}

	// Down-Right
	if( (command->vel.x < 0.0f) &&
		(command->vel.y > 0.0f) )
	{
		command->heading = SOUTHWEST;
	}

	// Up-Right
	if( (command->vel.x < 0.0f) &&
		(command->vel.y < 0.0f) )
	{
		command->heading = NORTHWEST;
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::CalculateBulletVelocity(command_t *command)
{
	command->bullet.shot = true;

	if(command->heading == NORTH)
	{
		command->bullet.vel.x = 0.0f;
		command->bullet.vel.y = -200.0f;
	}
	if(command->heading == SOUTH)
	{
		command->bullet.vel.x = 0.0f;
		command->bullet.vel.y = 200.0f;
	}
	if(command->heading == EAST)
	{
		command->bullet.vel.x = 200.0f;
		command->bullet.vel.y = 0.0f;
	}
	if(command->heading == WEST)
	{
		command->bullet.vel.x = -200.0f;
		command->bullet.vel.y = 0.0f;
	}

	if(command->heading == NORTHEAST)
	{
		command->bullet.vel.x = 200.0f;
		command->bullet.vel.y = -200.0f;
	}
	if(command->heading == NORTHWEST)
	{
		command->bullet.vel.x = -200.0f;
		command->bullet.vel.y = -200.0f;
	}
	if(command->heading == SOUTHEAST)
	{
		command->bullet.vel.x = 200.0f;
		command->bullet.vel.y = 200.0f;
	}
	if(command->heading == SOUTHWEST)
	{
		command->bullet.vel.x = -200.0f;
		command->bullet.vel.y = 200.0f;
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::MovePlayer(clientData *client)
{
	float clientFrametime;

	float multiplier = 100.0f;

	clientFrametime = client->command.msec / 1000.0f;;

	CalculateVelocity(&client->command, clientFrametime);
	CalculateHeading(&client->command);

	// Move the client based on the commands
	client->command.origin.x += client->command.vel.x;
	client->command.origin.y += client->command.vel.y;

	// Bullet
	if(client->command.bullet.shot == false)
	{
		client->command.bullet.origin.x = client->command.origin.x;
		client->command.bullet.origin.y = client->command.origin.y;
	}
	else
	{
		client->command.bullet.lifetime += (int) (clientFrametime * 1000.0f);

		if(client->command.bullet.lifetime > 2000)
		{
			client->command.bullet.shot = false;
			client->command.bullet.lifetime = 0;

			client->command.bullet.origin.x = client->command.origin.x;
			client->command.bullet.origin.y = client->command.origin.y;
		}
	}

	if(client->command.key & KEY_WEAPON && client->command.bullet.shot == false)
	{
		CalculateBulletVelocity(&client->command);
	}

	if(client->command.bullet.shot)
	{
		client->command.bullet.origin.x += client->command.bullet.vel.x * clientFrametime;
		client->command.bullet.origin.y += client->command.bullet.vel.y * clientFrametime;
	}

	// Check for bullet hits
	if(client->command.bullet.shot)
	{
		for(clientData *client2 = clientList; client2 != NULL; client2 = client2->next)
		{
			if(client == client2)
				continue;

			client2->diedThisFrame = false;

			VECTOR2D pos = client2->command.origin;
			pos.x += 16.0f;
			pos.y += 16.0f;

			VECTOR2D vec = VectorSubstract(&client->command.bullet.origin, &pos);
			float distance = VectorLength(&vec);

			if(distance < 16.0f)
			{
				// Player dies
				client2->command.origin.x = client2->startPos.x;
				client2->command.origin.y = client2->startPos.y;
				client2->diedThisFrame = true;

				if(client2 == playerWithFlag)
				{
					playerWithFlag = NULL;
					updateFlag = true;
				}

				updateKill = true;

				client->command.bullet.shot = false;

				break;
			}
		}
	}

	int f = client->netClient->GetIncomingSequence() & (COMMAND_HISTORY_SIZE-1);
	client->processedFrame = f;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::CheckFlagCollisions(void)
{
	if(playerWithFlag != NULL)
	{
		// Move the flag with the player
		flagX = playerWithFlag->command.origin.x;
		flagY = playerWithFlag->command.origin.y;
	
		// Check if the player is at home base
		if( (playerWithFlag) && (playerWithFlag->team == BLUE_TEAM) )
		{
			if(playerWithFlag->command.origin.x+16 > (49*32) &&
				playerWithFlag->command.origin.x+16 < (50*32) &&
				playerWithFlag->command.origin.y+16 > (3*32) &&
				playerWithFlag->command.origin.y+16 < (4*32))
			{
				flagX = 49*32;
				flagY = 49*32;
				playerWithFlag = NULL;
				blueScore++;

				updateFlag = true;
			}
		}
		if( (playerWithFlag) && (playerWithFlag->team == RED_TEAM) )
		{
			if(playerWithFlag->command.origin.x+16 > (49*32) &&
				playerWithFlag->command.origin.x+16 < (50*32) &&
				playerWithFlag->command.origin.y+16 > (97*32) &&
				playerWithFlag->command.origin.y+16 < (98*32))
			{
				flagX = 49*32;
				flagY = 49*32;
				playerWithFlag = NULL;
				redScore++;

				updateFlag = true;
			}
		}
	}
	else
	{
		// Check if anyone is in contact with the flag
		clientData *list = clientList;

		for( ; list != NULL; list = list->next)
		{
			if(list->command.origin.x+16 > flagX && list->command.origin.x+16 < flagX+32 &&
				list->command.origin.y+16 > flagY && list->command.origin.y+16 < flagY+32)
			{
				char team[10];

				if(list->team == RED_TEAM)
					strcpy(team, "RED team");
				else
					strcpy(team, "BLUE team");

				LogString("FLAG hit : player %d: %s, %s", list->netClient->GetIndex(), list->netClient->GetName(), team);

				playerWithFlag = list;

				updateFlag = true;
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::AddClient(void)
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

		if(clients % 2 == 0)
		{
			clientList->team = RED_TEAM;

			clientList->startPos.x = 46.0f * 32.0f + ((clients/2) * 32.0f);
			clientList->startPos.y = 96.0f * 32.0f;
		}
		else
		{
			clientList->team = BLUE_TEAM;

			clientList->startPos.x = 46.0f * 32.0f + ((clients/2) * 32.0f);
			clientList->startPos.y = 4.0f * 32.0f;
		}

		clientList->command.origin.x = clientList->startPos.x;
		clientList->command.origin.y = clientList->startPos.y;

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

		if(clients % 2 == 0)
		{
			list->team = RED_TEAM;

			list->startPos.x = 46.0f * 32.0f + ((clients/2) * 32.0f);
			list->startPos.y = 96.0f * 32.0f;
		}
		else
		{
			list->team = BLUE_TEAM;

			list->startPos.x = 46.0f * 32.0f + ((clients/2) * 32.0f);
			list->startPos.y = 4.0f * 32.0f;
		}

		list->command.origin.x = list->startPos.x;
		list->command.origin.y = list->startPos.y;

		list->next = NULL;

		prev->next = list;
	}

	clients++;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::RemoveClient(struct sockaddr *address)
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

	// Drop the flag if player with flag exits the game
	if(list == playerWithFlag)
	{
		playerWithFlag = NULL;
		updateFlag = true;
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

	clients--;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWarServer::RemoveClients(void)
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
void CArmyWarServer::Frame(int msec)
{
	realtime += msec;
	frametime = msec / 1000.0f;

	// Read packets from clients
	ReadPackets();

	if(inProgress == false)
		return;

	// Check if someone hit the flag
	CheckFlagCollisions();

	// Wait full 100 ms before allowing to send
	if(realtime < servertime)
	{
		// never let the time get too far off
		if(servertime - realtime > 100)
		{
			realtime = servertime - 100;
		}

		return;
	}

	// Bump frame number, and calculate new servertime
	framenum++;
	servertime = framenum * 100;

	if(servertime < realtime)
		realtime = servertime;

	SendCommand();

	// Reset update flags
	updateFlag = false;
	updateKill = false;
}
