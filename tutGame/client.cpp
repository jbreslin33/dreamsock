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
CArmyWar::CArmyWar()
{
	networkClient	= new dreamClient;
	clientList		= NULL;
	localClient		= NULL;
	clients			= 0;

	memset(&inputClient, 0, sizeof(clientData));
	memset(&mapdata, 0, sizeof(mapdata));

	frametime		= 0.0f;

	inProgress		= false;
	init			= false;

	scrollX			= 0;
	scrollY			= 0;

	tileScrollX		= 0;
	tileScrollY		= 0;

	gameIndex		= 0;

	targetRotation	= 0.0f;

	redScore		= 0;
	blueScore		= 0;

	playerWithFlag	= NULL;

	next			= NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
CArmyWar::~CArmyWar()
{
	delete networkClient;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::InitialiseEngine(void)
{
	// Init the Graphics Engine
	GFX_Init("Army War Engine v2.0", 640, 480, 16, 0, ApplicationProc);

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

	// Set the scroll positions
	scrollX = 40*32;

	if(localClient->team == RED_TEAM)
		scrollY = 90*32;
	else
		scrollY = 0;

	// Set the flag position
	flagX = 49*32;
	flagY = 49*32;

	playerWithFlag = NULL;

	// Reset score counters
	redScore	= 0;
	blueScore	= 0;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::DrawMap(void)
{
	int heading = 0;

	 // Work out how many tiles have been scrolled
   	tileScrollX = scrollX/32;
   	tileScrollY = scrollY/32;

	for(int i = (tileScrollX)-2; i < (tileScrollX)+21; i++)
   	{
   		for(int j = (tileScrollY)+15; j > (tileScrollY)-2; j--)
   		{
 			GFX_Blit(&grass, (32*i)-(scrollX),(32*j)-(scrollY), 32, 32, 0);

			// Draw a tree if required
			if(mapdata[i][j] == true)
			{
				GFX_Blit(&tree, (32*i+16)-(scrollX),(32*j+16)-(scrollY), 32, 32, 0);
			}
			
			// Draw the static targets
			if(i==49 && j==3)
			{
				// draw the blue target
				GFX_Blit(&bluetarget, (32*i)-(scrollX),(32*j)-(scrollY),
					32, 32, targetRotation);
			}

			if(i==49 && j==97)
			{
				// draw the blue target
				GFX_Blit(&redtarget, (32*i)-(scrollX),(32*j)-(scrollY),
					32, 32, targetRotation);
			}
		}
	}

	// Render the flag
	GFX_Blit(&flag, ((int) flagX)-(scrollX), ((int) flagY)-(scrollY), 32, 32, 0);

	// Render players
	clientData *list = clientList;

	for( ; list != NULL; list = list->next)
	{
		if(list->team == RED_TEAM)
		{
			GFX_Blit(&redman, ((int) list->command.origin.x)-(scrollX),
				((int) list->command.origin.y)-(scrollY),
				32, 32, (float) list->command.heading);
		}

		if(list->team == BLUE_TEAM)
		{
			GFX_Blit(&blueman, ((int) list->command.origin.x)-(scrollX),
				((int) list->command.origin.y)-(scrollY),
				32, 32, (float) list->command.heading);
		}

		// Render bullets
		if(list->command.bullet.shot)
		{
			if(list->team == RED_TEAM)
			{
				GFX_RectFill(((int) list->command.bullet.origin.x-2)-(scrollX),
					((int) list->command.bullet.origin.y-2)-(scrollY),
					((int) list->command.bullet.origin.x+2)-(scrollX),
					((int) list->command.bullet.origin.y+2)-(scrollY), 200, 0, 0);
			}

			if(list->team == BLUE_TEAM)
			{
				GFX_RectFill(((int) list->command.bullet.origin.x-2)-(scrollX),
					((int) list->command.bullet.origin.y-2)-(scrollY),
					((int) list->command.bullet.origin.x+2)-(scrollX),
					((int) list->command.bullet.origin.y+2)-(scrollY), 0, 0, 200);
			}
		}
	}

	// Finally, Render the Team Scores
	GFX_Blit(&rednumbers[redScore], 5, 410, 64, 64, 0);
	GFX_Blit(&bluenumbers[blueScore], 570, 410, 64, 64, 0);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::Frame(void)
{
	if(!localClient)
		return;

	// Scroll the map to follow the local player
	if((localClient->command.origin.x - scrollX) > 340)
	{
		if(scrollX <= 3200-(19*32)-2)
			scrollX += 2;
	}

	if((localClient->command.origin.x - scrollX) < 300)
	{
		if(scrollX >= 2)
			scrollX -= 2;
	}

	if((localClient->command.origin.y - scrollY) > 260)
	{
		if(scrollY <= 3200-(15*32)-2)
			scrollY += 2;
	}

	if((localClient->command.origin.y - scrollY) < 220)
	{
		if(scrollY >= 2)
			scrollY -= 2;
	}

	// Move the flag with the player
	if(playerWithFlag)
	{
		flagX = playerWithFlag->command.origin.x;
		flagY = playerWithFlag->command.origin.y;
	}

	// Rotate the target images
	if(targetRotation < 360)
		targetRotation += 1;
	else
		targetRotation -= targetRotation;

	// Draw map
	GFX_Begin();
	{
		DrawMap();
	}
	GFX_End();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::Shutdown(void)
{
	Disconnect();

	GFX_Begin();
	GFX_Shutdown();
	GFX_End();
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::CheckVictory(void)
{
	if(localClient == NULL)
		return;

	// Check team scores
	if(redScore > 1)
	{
		if(localClient->team == RED_TEAM)
		{
			MessageBox(NULL, "Your team (RED) won!", "Victory", MB_OK);
		}
		else
		{
			MessageBox(NULL, "The other team (RED) won", "Failure", MB_OK);
		}

		Shutdown();
	}
	if(blueScore > 1)
	{
		if(localClient->team == BLUE_TEAM)
		{
			MessageBox(NULL, "Your team (BLUE) won!", "Victory", MB_OK);
		}
		else
		{
			MessageBox(NULL, "The other team (BLUE) won", "Failure", MB_OK);
		}

		Shutdown();
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::KillPlayer(int index)
{
	LogString("Player %d died", index);

	clientData *client = GetClientPointer(index);

	if(client == NULL)
		return;

	client->command.origin.x = client->startPos.x;
	client->command.origin.y = client->startPos.y;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
clientData *CArmyWar::GetClientPointer(int index)
{
	for(clientData *clList = clientList; clList != NULL; clList = clList->next)
	{
		if(clList->index == index)
			return clList;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::CheckKeys(void)
{
	inputClient.command.key = 0;

	if(keys[VK_ESCAPE])
	{
		Shutdown();

		keys[VK_ESCAPE] = false;
	}

	if(keys[VK_DOWN])
	{
		inputClient.command.key |= KEY_DOWN;
	}

	if(keys[VK_UP])
	{
		inputClient.command.key |= KEY_UP;
	}

	if(keys[VK_LEFT])
	{
		inputClient.command.key |= KEY_LEFT;
	}

	if(keys[VK_RIGHT])
	{
		inputClient.command.key |= KEY_RIGHT;
	}

	if(keys[VK_SPACE])
	{
		inputClient.command.key |= KEY_WEAPON;
	}

	inputClient.command.msec = (int) (frametime * 1000);
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::CheckPredictionError(int a)
{
	if(a < 0 && a > COMMAND_HISTORY_SIZE)
		return;

	float errorX =
		localClient->serverFrame.origin.x - localClient->frame[a].predictedOrigin.x;
		
	float errorY =
		localClient->serverFrame.origin.y - localClient->frame[a].predictedOrigin.y;

	// Fix the prediction error
	if( (errorX != 0.0f) || (errorY != 0.0f) )
	{
		localClient->frame[a].predictedOrigin.x = localClient->serverFrame.origin.x;
		localClient->frame[a].predictedOrigin.y = localClient->serverFrame.origin.y;

		localClient->frame[a].vel.x = localClient->serverFrame.vel.x;
		localClient->frame[a].vel.y = localClient->serverFrame.vel.y;

		LogString("Prediction error for frame %d:     %f, %f\n", a,
			errorX, errorY);
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::CheckBulletPredictionError(int a)
{
	if(a < 0 && a > COMMAND_HISTORY_SIZE)
		return;

	float errorX = localClient->serverFrame.bullet.origin.x - localClient->frame[a].bullet.predictedOrigin.x;
	float errorY = localClient->serverFrame.bullet.origin.y - localClient->frame[a].bullet.predictedOrigin.y;

	// Fix the prediction error
	if( (errorX != 0.0f) || (errorY != 0.0f) )
	{
		localClient->frame[a].bullet.predictedOrigin.x = localClient->serverFrame.bullet.origin.x;
		localClient->frame[a].bullet.predictedOrigin.y = localClient->serverFrame.bullet.origin.y;

		localClient->frame[a].bullet.vel.x = localClient->serverFrame.bullet.vel.x;
		localClient->frame[a].bullet.vel.y = localClient->serverFrame.bullet.vel.y;

		LogString("Bullet prediction error for frame %d:     %f, %f\n", a,
			errorX, errorY);
	}
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::CalculateVelocity(command_t *command, float frametime)
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
void CArmyWar::CalculateHeading(command_t *command)
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
void CArmyWar::CalculateBulletVelocity(command_t *command)
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
void CArmyWar::PredictMovement(int prevFrame, int curFrame)
{
	if(!localClient)
		return;

	float frametime = inputClient.frame[curFrame].msec / 1000.0f;

	localClient->frame[curFrame].key = inputClient.frame[curFrame].key;

	//
	// Player ->
	//
	
	// Process commands
	CalculateVelocity(&localClient->frame[curFrame], frametime);
	CalculateHeading(&localClient->frame[curFrame]);

	// Calculate new predicted origin
	localClient->frame[curFrame].predictedOrigin.x =
		localClient->frame[prevFrame].predictedOrigin.x + localClient->frame[curFrame].vel.x;

	localClient->frame[curFrame].predictedOrigin.y =
		localClient->frame[prevFrame].predictedOrigin.y + localClient->frame[curFrame].vel.y;

	// Copy values to "current" values
	localClient->command.predictedOrigin.x	= localClient->frame[curFrame].predictedOrigin.x;
	localClient->command.predictedOrigin.y	= localClient->frame[curFrame].predictedOrigin.y;
	localClient->command.vel.x				= localClient->frame[curFrame].vel.x;
	localClient->command.vel.y				= localClient->frame[curFrame].vel.y;
	localClient->command.heading			= localClient->frame[curFrame].heading;


	//
	// Bullet ->
	//

	// First set the previous values
	localClient->frame[curFrame].bullet.shot = localClient->frame[prevFrame].bullet.shot;
	localClient->frame[curFrame].bullet.vel.x = localClient->frame[prevFrame].bullet.vel.x;
	localClient->frame[curFrame].bullet.vel.y = localClient->frame[prevFrame].bullet.vel.y;
	localClient->frame[curFrame].bullet.lifetime = localClient->frame[prevFrame].bullet.lifetime;

	// The bullet is carried by the player
	if(localClient->frame[curFrame].bullet.shot == false)
	{
		localClient->frame[curFrame].bullet.predictedOrigin.x = localClient->frame[curFrame].predictedOrigin.x;
		localClient->frame[curFrame].bullet.predictedOrigin.y = localClient->frame[curFrame].predictedOrigin.y;
	}
	else
	{
		localClient->frame[curFrame].bullet.lifetime += (int) (frametime * 1000.0f);

		if(localClient->frame[curFrame].bullet.lifetime > 2000)
		{
			localClient->frame[curFrame].bullet.shot = false;
			localClient->frame[curFrame].bullet.lifetime = 0;

			localClient->frame[curFrame].bullet.predictedOrigin.x = localClient->frame[curFrame].predictedOrigin.x;
			localClient->frame[curFrame].bullet.predictedOrigin.y = localClient->frame[curFrame].predictedOrigin.y;
		}
	}

	// Calculate the heading for the bullet only when firing
	if(localClient->frame[curFrame].key & KEY_WEAPON && localClient->frame[curFrame].bullet.shot == false)
	{
		CalculateBulletVelocity(&localClient->frame[curFrame]);
	}

	// If the bullet is in the air (shot), update its origin
	if(localClient->frame[curFrame].bullet.shot)
	{
		localClient->frame[curFrame].bullet.predictedOrigin.x =
			localClient->frame[prevFrame].bullet.predictedOrigin.x +
			localClient->frame[curFrame].bullet.vel.x * frametime;

		localClient->frame[curFrame].bullet.predictedOrigin.y =
			localClient->frame[prevFrame].bullet.predictedOrigin.y +
			localClient->frame[curFrame].bullet.vel.y * frametime;
	}

	// Copy values to "current" values
	localClient->command.bullet.predictedOrigin.x	= localClient->frame[curFrame].bullet.predictedOrigin.x;
	localClient->command.bullet.predictedOrigin.y	= localClient->frame[curFrame].bullet.predictedOrigin.y;
	localClient->command.bullet.vel.x				= localClient->frame[curFrame].bullet.vel.x;
	localClient->command.bullet.vel.y				= localClient->frame[curFrame].bullet.vel.y;
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void CArmyWar::MoveObjects(void)
{
	if(!localClient)
		return;

	clientData *client = clientList;

	for( ; client != NULL; client = client->next)
	{
		// Remote players
		if(client != localClient)
		{
			CalculateVelocity(&client->command, frametime);
			CalculateHeading(&client->command);

			client->command.origin.x += client->command.vel.x;
			client->command.origin.y += client->command.vel.y;

			client->command.bullet.origin.x += client->serverFrame.bullet.vel.x * frametime;
			client->command.bullet.origin.y += client->serverFrame.bullet.vel.y * frametime;
		}

		// Local player
		else
		{
			client->command.origin.x = client->command.predictedOrigin.x;
			client->command.origin.y = client->command.predictedOrigin.y;

			client->command.bullet.origin.x = client->command.bullet.predictedOrigin.x;
			client->command.bullet.origin.y = client->command.bullet.predictedOrigin.y;
		}
	}
}

