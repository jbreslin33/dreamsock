#include <dreamSock.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <2dlib.h>

#include "engine.h"
//#include "main.h"
//#include "network.h"
//#include "lobby.h"

//#include "..\tutGameServer\netcommon.h"

// Graphic Declarations
GFX_IMAGE2D grass;				// variable to hold 'cdrom' graphic
GFX_IMAGE2D redman;				// variable to hold 'cdrom' graphic
GFX_IMAGE2D blueman;			// variable to hold 'cdrom' graphic
GFX_IMAGE2D tree;				// variable to hold 'cdrom' graphic
GFX_IMAGE2D redtarget;			// variable to hold 'cdrom' graphic
GFX_IMAGE2D bluetarget;			// variable to hold 'cdrom' graphic
GFX_IMAGE2D flag;				// variable to hold 'cdrom' graphic
GFX_IMAGE2D rednumbers[10];		// variable to hold 'cdrom' graphic
GFX_IMAGE2D bluenumbers[10];	// variable to hold 'cdrom' graphic

float target_rotation = 0.0;	// variable to rotate the target images

// Define the map
bool mapdata[100][100];

// Tile scroll positions
int scroll_x, scroll_y;
int tile_scroll_x = 0;
int tile_scroll_y = 0;

// Flag Data
int flag_x;
int flag_y;

int local_x;		// Reference coordinate
int local_y;		// Reference coordinate
bool southeast;		// Direction flag
bool southwest;		// Direction flag
bool northeast;		// Direction flag
bool northwest;		// Direction flag
bool south;			// Direction flag
bool north;			// Direction flag
bool east;			// Direction flag
bool west;			// Direction flag
bool stopped;		// Direction flag

struct player_t players[16];	// limit of 16 players
int ingame_players;				// the number of players in the game
int half_players;				// half the amount of players in the game
int place_pos_x;
int player_with_flag_id;

struct bullet_t bullets[MAX_BULLETS]; // limit of 500 bullets (at once)

int red_score;
int blue_score;

int final_winning_team;

int engine_inited;
/*
void ENGINE_CheckBulletCollisions(void)
{
	for(int i=0;i<MAX_BULLETS;i++)
	{
		if(bullets[i].life_remaining > 0)
		{
			// Check if the bullet is in contact with an opposite team member.
			for(int j=0;j<ingame_players;j++)
			{
				if(players[j].team != bullets[i].team &&
					bullets[i].x > players[j].x &&
					bullets[i].x < players[j].x+32
					&& bullets[i].y > players[j].y &&
					bullets[i].y < players[j].y+32)
				{
					// Kill + Respawn the player
					players[j].x = players[j].start_x;
					players[j].y = players[j].start_y;

					if(localPlayerGameIndex == j)
					{
						local_x = players[j].x;
						local_y = players[j].y;

						scroll_x = local_x - 320;
						scroll_y = local_y - 240;
					}
					
					// Check if the player was holding the flag
					if(j == player_with_flag_id)
					{
						// Respawn the flag
						player_with_flag_id = -1;
						flag_x = 49*32;
						flag_y = 49*32;
					}

					break;
				}
			}
		}
	}
}


void ENGINE_CheckFlagCollisions(void)
{
	if(player_with_flag_id != -1)
	{
		// Move the flag with the player
		flag_x = players[player_with_flag_id].x;
		flag_y = players[player_with_flag_id].y;
	
		// Check if the player is at home base
		if(players[player_with_flag_id].team == BLUE_TEAM)
		{
			if(players[player_with_flag_id].x+16 > (49*32) &&
				players[player_with_flag_id].x+16 < (50*32) &&
				players[player_with_flag_id].y+16 > (3*32) &&
				players[player_with_flag_id].y+16 < (4*32))
			{
				flag_x = 49*32;
				flag_y = 49*32;
				player_with_flag_id = -1;
				blue_score++;
			}
		}
		if(players[player_with_flag_id].team == RED_TEAM)
		{
			if(players[player_with_flag_id].x+16 > (49*32) &&
				players[player_with_flag_id].x+16 < (50*32) &&
				players[player_with_flag_id].y+16 > (97*32) &&
				players[player_with_flag_id].y+16 < (98*32))
			{
				flag_x = 49*32;
				flag_y = 49*32;
				player_with_flag_id = -1;
				red_score++;
			}
		}
	}
	else
	{
		// Check if anyone is in contact with the flag
		for(int j=0;j<ingame_players;j++)
		{
			if(players[j].x+16>flag_x && players[j].x+16<flag_x+32 &&
				players[j].y+16>flag_y && players[j].y+16<flag_y+32)
			{
				player_with_flag_id = j;

				break;
			}
		}
	}

	// Check team scores
	if(red_score > 1)
	{
		if(localPlayerGameIndex == 0)
		{
			DoDestroyGame(Game[selectedGame].name);
		}

		final_winning_team = RED_TEAM;
		ENGINE_Shutdown();

		if(players[localPlayerGameIndex].team == RED_TEAM)
		{
			MessageBox(NULL, "Your team (RED) won!", "Victory", MB_OK);
		}
		else
		{
			MessageBox(NULL, "The other team (RED) won", "Failure", MB_OK);
		}
	}
	if(blue_score > 1)
	{
		if(localPlayerGameIndex == 0)
		{
			DoDestroyGame(Game[selectedGame].name);
		}

		final_winning_team = BLUE_TEAM;
		ENGINE_Shutdown();

		if(players[localPlayerGameIndex].team == BLUE_TEAM)
		{
			MessageBox(NULL, "Your team (BLUE) won!", "Victory", MB_OK);
		}
		else
		{
			MessageBox(NULL, "The other team (BLUE) won", "Failure", MB_OK);
		}
	}
}

void ENGINE_AddBullet(int x,int y,int dir,int team)
{
	// find an empty slot in the array
	int empty_id = -1;
	for(int i=0;i<MAX_BULLETS;i++)
	{
		if(bullets[i].life_remaining <= 0)
		{
			empty_id = i;
			break;
		}
	}

	// If a slot has been found
	if(empty_id != -1)
	{
		// Add the bullet...
		bullets[empty_id].x              = x;
		bullets[empty_id].y              = y;
		bullets[empty_id].dir            = dir;
		bullets[empty_id].life_remaining = 30;
		bullets[empty_id].team           = team;
	}
}

void ENGINE_UpdateBullets(void)
{
	for(int i=0;i<MAX_BULLETS;i++)
	{
		if(bullets[i].life_remaining > 0)
		{
			switch(bullets[i].dir)
			{
			case NORTH:
				bullets[i].y-=4;
				break;
			case NORTHEAST:
				bullets[i].x+=4;
				bullets[i].y-=4;
				break;
			case EAST:
				bullets[i].x+=4;
				break;
			case SOUTHEAST:
				bullets[i].x+=4;
				bullets[i].y+=4;
				break;
			case SOUTH:
				bullets[i].y+=4;
				break;
			case SOUTHWEST:
				bullets[i].x-=4;
				bullets[i].y+=4;
				break;
			case WEST:
				bullets[i].x-=4;
				break;
			case NORTHWEST:
				bullets[i].x-=4;
				bullets[i].y-=4;
				break;
			}
			bullets[i].life_remaining--;
		}
	}
}
*/

void ENGINE_AddPlayer(int team)
{
	if(ingame_players < 16)
	{
		players[ingame_players].dx = 0;
		players[ingame_players].dy = 0;

		players[ingame_players].x = place_pos_x*32;

		if(team == BLUE_TEAM)
		{
			players[ingame_players].y = 4*32;
			players[ingame_players].dir = SOUTH;
		}
		else
		{
			players[ingame_players].y = 96*32;
			players[ingame_players].dir = NORTH;
		}
		
		players[ingame_players].start_x = players[ingame_players].x; 
		players[ingame_players].start_y = players[ingame_players].y;
		players[ingame_players].team = team;

		ingame_players++;
		place_pos_x++;
	}
}


void ENGINE_Init(void)
{
	// Init the Graphics Engine
	GFX_Init("Army War Engine v1.0", 640, 480, 16, 0, NULL);
//	GFX_Init("Army War Engine v1.0", 640, 480, 16, 0, ApplicationProc);

	// Load Required Graphics
	GFX_LoadBitmap(&grass,      "gfx\\grass.bmp");	
	GFX_LoadBitmap(&redman,     "gfx\\redman.bmp");	
	GFX_LoadBitmap(&blueman,    "gfx\\blueman.bmp");
	GFX_LoadBitmap(&tree,       "gfx\\tree.bmp");
	GFX_LoadBitmap(&redtarget,  "gfx\\redtarget.bmp");
	GFX_LoadBitmap(&bluetarget, "gfx\\bluetarget.bmp");
	GFX_LoadBitmap(&flag,       "gfx\\flag.bmp");

	GFX_LoadBitmap(&rednumbers[0], "gfx\\red0.bmp");
	GFX_LoadBitmap(&rednumbers[1], "gfx\\red1.bmp");
	GFX_LoadBitmap(&rednumbers[2], "gfx\\red2.bmp");
	GFX_LoadBitmap(&rednumbers[3], "gfx\\red3.bmp");
	GFX_LoadBitmap(&rednumbers[4], "gfx\\red4.bmp");
	GFX_LoadBitmap(&rednumbers[5], "gfx\\red5.bmp");
	GFX_LoadBitmap(&rednumbers[6], "gfx\\red6.bmp");
	GFX_LoadBitmap(&rednumbers[7], "gfx\\red7.bmp");
	GFX_LoadBitmap(&rednumbers[8], "gfx\\red8.bmp");
	GFX_LoadBitmap(&rednumbers[9], "gfx\\red9.bmp");

	GFX_LoadBitmap(&bluenumbers[0], "gfx\\blue0.bmp");
	GFX_LoadBitmap(&bluenumbers[1], "gfx\\blue1.bmp");
	GFX_LoadBitmap(&bluenumbers[2], "gfx\\blue2.bmp");
	GFX_LoadBitmap(&bluenumbers[3], "gfx\\blue3.bmp");
	GFX_LoadBitmap(&bluenumbers[4], "gfx\\blue4.bmp");
	GFX_LoadBitmap(&bluenumbers[5], "gfx\\blue5.bmp");
	GFX_LoadBitmap(&bluenumbers[6], "gfx\\blue6.bmp");
	GFX_LoadBitmap(&bluenumbers[7], "gfx\\blue7.bmp");
	GFX_LoadBitmap(&bluenumbers[8], "gfx\\blue8.bmp");
	GFX_LoadBitmap(&bluenumbers[9], "gfx\\blue9.bmp");
	
	// Create the map...
//	if(localPlayerGameIndex == 0)
	{
		ENGINE_GenerateRandomMap();

		// Send map details to other players...
/*		TUT_MSG_MAPINFO_DATA MapMsg;

		MapMsg.type		= TUT_MSG_MAPINFO;
		MapMsg.toId		= NETID_ALL;
		MapMsg.fromId	= serverSocket.localId;
		memcpy(MapMsg.map, mapdata, sizeof(mapdata));

		sendGameMessage((NETMSG_GENERIC *) &MapMsg);
*/
	}

	// Create the players...
	
	// Set currently assigned players to 0
	ingame_players = 0;

	// Divide the players between the two teams
	int i; // loop variable

		// Add blue players
		place_pos_x = 46;
		half_players = Game[selectedGame].players / 2;
		for(i=0;i<half_players;i++)
		{
			ENGINE_AddPlayer(BLUE_TEAM);
		}			
	
		// Add red players
		place_pos_x = 46;
		half_players = Game[selectedGame].players - half_players;
		for(i=0;i<half_players;i++)
		{
			ENGINE_AddPlayer(RED_TEAM);
		}

	local_x = players[localPlayerGameIndex].start_x;
	local_y = players[localPlayerGameIndex].start_y;

	southeast = 0;
	southwest = 0;
	northeast = 0;
	northwest = 0;
	south = 0;
	north = 0;
	east = 0;
	west = 0;
	stopped = 0;

	// Set the scroll positions
	scroll_x = 40*32;
	if(players[localPlayerGameIndex].team == RED_TEAM)
	{
		scroll_y = 90*32;
	}
	else
	{
		scroll_y = 0;
	}

	// Set the flag position
	flag_x = 49*32;
	flag_y = 49*32;
	player_with_flag_id = -1;

	// Reset Score Counters
	red_score = 0;
	blue_score = 0;

	// Set the engine as ready
	engine_inited = 1;
}

void ENGINE_ProcessInput(void)
{
	TUT_MSG_PLAYERDIRECTION_DATA Msg;
	Msg.type		= TUT_MSG_PLAYERDIRECTION;
	Msg.toId		= NETID_ALL;
	Msg.fromId		= serverSocket.localId;
	Msg.playerIndex	= localPlayerGameIndex;

	TUT_MSG_PLAYERADDBULLET_DATA BulMsg;
	BulMsg.type			= TUT_MSG_PLAYERADDBULLET;
	BulMsg.toId			= NETID_ALL;
	BulMsg.fromId		= serverSocket.localId;
	BulMsg.playerIndex	= localPlayerGameIndex;

	if(keys[VkKeyScan('q')])
	{
		LogString("Q pressed");

		// Host
		if(localPlayerGameIndex == 0)
		{
			DoDestroyGame(Game[selectedGame].name);
		}
		else
		{
			DoLogoffGame();
		}

		ENGINE_Shutdown();

		return;
	}

		// Fire Button
	if(keys[VK_SPACE] && players[localPlayerGameIndex].fire_delay == 0)
	{
		players[localPlayerGameIndex].fire_delay = 20;

		sendPlayerMessage((NETMSG_GENERIC *) &BulMsg);
	}

	// SOUTHEAST
	if(keys[VK_DOWN] && keys[VK_RIGHT])
	{
		if(!southeast)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 1;
			Msg.east	= 1;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 1;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 0;
			east = 0;
			west = 0;
		}

		local_x += 2;
		local_y += 2;
	}
	// SOUTHWEST
	else if(keys[VK_DOWN] && keys[VK_LEFT])
	{
		if(!southwest)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 1;
			Msg.east	= 0;
			Msg.west	= 1;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 1;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 0;
			east = 0;
			west = 0;
		}

		local_x -= 2;
		local_y += 2;
	}
	// NORTHEAST
	else if(keys[VK_UP] && keys[VK_RIGHT])
	{
		if(!northeast)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 1;
			Msg.south	= 0;
			Msg.east	= 1;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 1;
			northwest = 0;
			south = 0;
			north = 0;
			east = 0;
			west = 0;
		}

		local_x += 2;
		local_y -= 2;
	}
	// NORTHWEST
	else if(keys[VK_UP] && keys[VK_LEFT])
	{
		if(!northwest)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 1;
			Msg.south	= 0;
			Msg.east	= 0;
			Msg.west	= 1;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 1;
			south = 0;
			north = 0;
			east = 0;
			west = 0;
		}

		local_x -= 2;
		local_y -= 2;
	}
	// SOUTH
	else if(keys[VK_DOWN])
	{
		if(!south)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 1;
			Msg.east	= 0;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 1;
			north = 0;
			east = 0;
			west = 0;
		}

		local_y += 2;
	}
	// NORTH
	else if(keys[VK_UP])
	{
		if(!north)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 1;
			Msg.south	= 0;
			Msg.east	= 0;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 1;
			east = 0;
			west = 0;
		}

		local_y -= 2;
	}
	// EAST
	else if(keys[VK_RIGHT])
	{
		if(!east)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 0;
			Msg.east	= 1;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 0;
			east = 1;
			west = 0;
		}

		local_x += 2;

		return;
	}
	// WEST
	else if(keys[VK_LEFT])
	{
		if(!west)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 0;
			Msg.east	= 0;
			Msg.west	= 1;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 0;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 0;
			east = 0;
			west = 1;
		}

		local_x -= 2;
	}
	// STOPPED
	else
	{
		if(!stopped)
		{
			Msg.x		= local_x;
			Msg.y		= local_y;
			Msg.north	= 0;
			Msg.south	= 0;
			Msg.east	= 0;
			Msg.west	= 0;

			sendPlayerMessage((NETMSG_GENERIC *) &Msg);

			stopped = 1;
			southeast = 0;
			southwest = 0;
			northeast = 0;
			northwest = 0;
			south = 0;
			north = 0;
			east = 0;
			west = 0;
		}
	}
}

void ENGINE_UpdatePlayers(void)
{
	// Loop through all the players
	for(int i = 0; i < ingame_players; i++)
	{
		if(players[i].dir == SOUTHEAST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x+32+2)/32][(players[i].y+2+32)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == SOUTHWEST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x-2)/32][(players[i].y+2+32)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == NORTHEAST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x+32+2)/32][(players[i].y-2)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == NORTHWEST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x-2)/32][(players[i].y-2)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == SOUTH)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x+16)/32][(players[i].y+2+32)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == NORTH)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x+16)/32][(players[i].y-2)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_y = players[i].y;
				}
				continue;
			}
		}

		if(players[i].dir == EAST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x+32+2)/32][(players[i].y+16)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
				}
				continue;
			}
		}

		if(players[i].dir == WEST)
		{
			// Check collision with a tree
			if(mapdata[(players[i].x-2)/32][(players[i].y+16)/32])
			{
				if(i == localPlayerGameIndex)
				{
					local_x = players[i].x;
				}
				continue;
			}
		}

		// Move players
		players[i].x += players[i].dx;
		players[i].y += players[i].dy;
	}

	// Scroll the map to follow the local player
	if((players[localPlayerGameIndex].x-scroll_x) > 340)
	{
		if(scroll_x <= 3200-(19*32)-2)
			scroll_x+=2;
	}
	if((players[localPlayerGameIndex].x-scroll_x) < 300)
	{
		if(scroll_x>=2)
			scroll_x-=2;
	}
	if((players[localPlayerGameIndex].y-scroll_y) > 260)
	{
		if(scroll_y <= 3200-(15*32)-2)
			scroll_y+=2;
	}
	if((players[localPlayerGameIndex].y-scroll_y) < 220)
	{
		if(scroll_y>=2)
			scroll_y-=2;
	}

	// Fire Delay
	if(players[localPlayerGameIndex].fire_delay > 0)
		players[localPlayerGameIndex].fire_delay--;
}

void ENGINE_Render(void)
{
	if(engine_inited == 1)
	{
		GFX_Begin();
		{
			ENGINE_DrawMap();
		}
		GFX_End();
		
		ENGINE_ProcessInput();
		ENGINE_UpdatePlayers();
		ENGINE_UpdateBullets();
		ENGINE_CheckBulletCollisions();
		ENGINE_CheckFlagCollisions();
	}
}


void ENGINE_GenerateRandomMap(void)
{
	// Make all land passable
	for(int i=0;i<100;i++)
	{
		for(int j=0;j<100;j++)
		{
			mapdata[i][j] = 0;
		}
	}

	// Place some random trees (avoiding the players start locations)
	for(i=0;i<300;i++)
	{
		mapdata[rand()%100][(rand()%80)+10] = 1;
	}
}

void ENGINE_DrawMap(void)
{
	 // Work out how many tiles have been scrolled
   	tile_scroll_x = scroll_x/32;
   	tile_scroll_y = scroll_y/32;

	for(int i=(tile_scroll_x)-2;i<(tile_scroll_x)+21;i++)
   	{
   		for(int j=(tile_scroll_y)+15;j>(tile_scroll_y)-2;j--)
   		{
 			GFX_Blit(&grass, (32*i)-(scroll_x),(32*j)-(scroll_y), 32, 32, 0);
			
			// Draw a tree if required
			if(mapdata[i][j] == 1)
			{
				GFX_Blit(&tree, (32*i)-(scroll_x),(32*j)-(scroll_y), 32, 32, 0);
			}
			
			// Draw the static targets
			if(i==49 && j==3)
			{
				// draw the blue target
				GFX_Blit(&bluetarget, (32*i)-(scroll_x),(32*j)-(scroll_y),
					32, 32, target_rotation);
			}
			if(i==49 && j==97)
			{
				// draw the blue target
				GFX_Blit(&redtarget, (32*i)-(scroll_x),(32*j)-(scroll_y),
					32, 32, target_rotation);
			}
		}
	}

	// Render the flag
	GFX_Blit(&flag, (flag_x)-(scroll_x),(flag_y)-(scroll_y), 32, 32, 0);

	// Render the players
	for(i=0;i<ingame_players;i++)
	{
		if(players[i].team == RED_TEAM)
		{
			GFX_Blit(&redman, (players[i].x)-(scroll_x),(players[i].y)-(scroll_y),
				32, 32, (float) players[i].dir);
		}
		if(players[i].team == BLUE_TEAM)
		{
			GFX_Blit(&blueman, (players[i].x)-(scroll_x),(players[i].y)-(scroll_y),
				32, 32, (float) players[i].dir);
		}
	}

	// Render Bullets
	for(i=0;i<MAX_BULLETS;i++)
	{
		if(bullets[i].life_remaining > 0)
		{
			if(bullets[i].team == RED_TEAM)
			{
				GFX_RectFill((bullets[i].x-2)-(scroll_x), (bullets[i].y-2)-(scroll_y),
					(bullets[i].x+2)-(scroll_x), (bullets[i].y+2)-(scroll_y), 200, 0, 0);
			}
			if(bullets[i].team == BLUE_TEAM)
			{
				GFX_RectFill((bullets[i].x-2)-(scroll_x), (bullets[i].y-2)-(scroll_y),
					(bullets[i].x+2)-(scroll_x), (bullets[i].y+2)-(scroll_y), 0, 0, 200);
			}
		}
	}

	// Finally, Render the Team Scores
	GFX_Blit(&rednumbers[red_score], 5, 410, 64, 64, 0);
	GFX_Blit(&bluenumbers[blue_score], 570, 410, 64, 64, 0);

	// Rotate the target
	if(target_rotation < 360)
		target_rotation += 1;
	else
		target_rotation -= target_rotation;
}

void ENGINE_Shutdown(void)
{
	if(engine_inited)
	{
		// Make sure all the keys are up
		keys[VK_UP]		= FALSE;
		keys[VK_DOWN]	= FALSE;
		keys[VK_RIGHT]	= FALSE;
		keys[VK_LEFT]	= FALSE;
		keys[VK_SPACE]	= FALSE;
		keys[VkKeyScan('q')] = FALSE;

		// Shutdown graphics engine
		GFX_Shutdown();
		engine_inited = 0;
	}
}
