// Define the map
extern bool mapdata[100][100];

// Tile scroll positions
extern int scroll_x, scroll_y;

// Player Data
#define BLUE_TEAM 0
#define RED_TEAM  1

#define NORTH     0
#define NORTHEAST 45
#define EAST      90
#define SOUTHEAST 135
#define SOUTH     180
#define SOUTHWEST 225
#define WEST      270
#define NORTHWEST 315

#define USMSG_STARTGAME		WM_USER + 2
#define USMSG_SHUTDOWNGAME	WM_USER + 3

extern int final_winning_team;

// Player Data
struct player_t
{
	int playerIndex;
	int x,y;
	int dx,dy;
	int start_x,start_y; // for re-spawning
	int dir;
	int team;
	int fire_delay;
};

// Bullet Data
struct bullet_t
{
	int x,y;
	int dir;
	int life_remaining;
	int team;
};
#define MAX_BULLETS 500

extern struct player_t players[16];	// limit of 16 players

void ENGINE_Init(void);
void ENGINE_Render(void);
void ENGINE_ProcessInput(void);
void ENGINE_GenerateRandomMap(void);
void ENGINE_DrawMap(void);
void ENGINE_Shutdown(void);
void ENGINE_AddPlayer(int team);
void ENGINE_AddBullet(int x,int y,int dir,int team);
void ENGINE_UpdateBullets(void);