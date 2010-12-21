#ifndef SERVER_H
#define SERVER_H

#include <string.h>

#define NORTH		0
#define NORTHEAST	45
#define EAST		90
#define SOUTHEAST	135
#define SOUTH		180
#define SOUTHWEST	225
#define WEST		270
#define NORTHWEST	315

#define BLUE_TEAM	0
#define RED_TEAM	1

typedef struct
{
	float x;
	float y;
} VECTOR2D;

typedef struct bullet_t
{
	VECTOR2D	vel;						// Velocity
	VECTOR2D	origin;						// Position

	bool		shot;						// Is the bullet in the air?
	int			lifetime;					// Lifetime in ms
} bullet_t;

typedef struct
{
	int key;								// Pressed keys
	int heading;							// Heading

	VECTOR2D vel;							// Velocity
	VECTOR2D origin;						// Position

	bullet_t bullet;						// Bullet

	int msec;								// How long to run command (in ms)
} command_t;

typedef struct clientData
{
	command_t frame[COMMAND_HISTORY_SIZE];
	command_t serverFrame;
	command_t command;

	long processedFrame;

	struct sockaddr address;
	dreamClient *netClient;

	VECTOR2D startPos;
	bool team;
	bool diedThisFrame;

	clientData *next;
} clientData;


class CArmyWarServer
{
private:
	dreamServer	*networkServer;

	clientData	*clientList;		// Client list
	int		clients;				// Number of clients

	int		realtime;				// Real server up-time in ms
	int		servertime;				// Server frame * 100 ms
	float	frametime;				// Frame time in seconds

	char	gamename[32];
	bool	inProgress;

	bool	mapdata[100][100];
	int		index;

	float	flagX;
	float	flagY;
	clientData *playerWithFlag;
	bool	updateFlag;
	bool	updateKill;

	int		redScore;
	int		blueScore;

	long	framenum;

public:
	CArmyWarServer();
	~CArmyWarServer();

	// Network.cpp
	void	ReadPackets(void);
	void	SendCommand(void);
	void	SendExitNotification(void);
	void	ReadDeltaMoveCommand(dreamMessage *mes, clientData *client);
	void	BuildMoveCommand(dreamMessage *mes, clientData *client);
	void	BuildDeltaMoveCommand(dreamMessage *mes, clientData *client);

	// Server.cpp
	int		InitNetwork(int gameAmount);
	void	ShutdownNetwork(void);
	void	GenerateRandomMap(void);
	void	CalculateVelocity(command_t *command, float frametime);
	void	CalculateHeading(command_t *command);
	void	CalculateBulletVelocity(command_t *command);
	void	MovePlayers(void);
	void	MovePlayer(clientData *client);
	void	CheckFlagCollisions(void);

	void	AddClient(void);
	void	RemoveClient(struct sockaddr *address);
	void	RemoveClients(void);
	void	Frame(int msec);

	clientData *GetClientList(void)	{ return clientList; }

	void	SetName(char *n)		{ strcpy(gamename, n); }
	char	*GetName(void)			{ return gamename; }

	void	SetInProgress(bool p)	{ inProgress = p; }	
	bool	GetInProgress(void)		{ return inProgress; }

	void	SetIndex(int ind)	{ index = ind; }
	int		GetIndex(void)		{ return index; }

	CArmyWarServer *next;
};

#endif
