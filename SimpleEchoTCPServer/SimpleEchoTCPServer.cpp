/*
	Simple echo TCP server
	Teijo Hakala 2001
*/

// Define _WINSOCKAPI_ so windows.h will not include old WinSock header.
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <windows.h>
#include <winsock2.h>

#include <stdio.h>

// Declare the sockets we use.
SOCKET listeningSocket;
SOCKET connectedSocket;

int dreamSock_InitializeWinSock(void)
{
	WORD versionRequested;
	WSADATA wsaData;
	DWORD bufferSize = 0;

	LPWSAPROTOCOL_INFO SelectedProtocol;
	int NumProtocols;

	// Start WinSock2. If it fails, we do not need to call WSACleanup()
	versionRequested = MAKEWORD(2, 0);
	int error = WSAStartup(versionRequested, &wsaData);
    
	if(error)
	{
		return 1;
	}
	else
	{
		// Confirm that the WinSock2 DLL supports the exact version
		// we want. If not, call WSACleanup().
		if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
		{
			WSACleanup();
			return 1;
		}
	}

	// Call WSAEnumProtocols to figure out how big of a buffer we need
	NumProtocols = WSAEnumProtocols(NULL, NULL, &bufferSize);

	if( (NumProtocols != SOCKET_ERROR) && (WSAGetLastError() != WSAENOBUFS) )
	{
		WSACleanup();
		return 1;
	}

	// Allocate a buffer, call WSAEnumProtocols to get an array of
	// WSAPROTOCOL_INFO structs
	SelectedProtocol = (LPWSAPROTOCOL_INFO) malloc(bufferSize);

	if(SelectedProtocol == NULL)
	{
		WSACleanup();
		return 1;
	}

	// Allocate memory for protocol list and define what protocols to look for
	int *protos = (int *) calloc(2, sizeof(int));

	protos[0] = IPPROTO_TCP;
	protos[1] = IPPROTO_UDP;

	NumProtocols = WSAEnumProtocols(protos, SelectedProtocol, &bufferSize);
	
	free(protos);
	protos = NULL;
	
	free(SelectedProtocol);
	SelectedProtocol = NULL;

	if(NumProtocols == SOCKET_ERROR)
	{
		WSACleanup();
		return 1;
	}

	return 0;
}

int InitSockets(void)
{
	struct sockaddr *servAddr;
	struct sockaddr_in *inetServAddr;

	int error = 0;

	// Create the socket.
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(listeningSocket == INVALID_SOCKET)
	{
		printf("error: socket() failed");
		return -1;
	}

	// Allocate memory for the address structure and set it to zero.
	servAddr = (struct sockaddr *) malloc(sizeof(sockaddr));
	memset((char *) servAddr, 0, sizeof(sockaddr));

	// Fill the address structure.
	servAddr->sa_family		= (u_short) AF_INET;
	inetServAddr			= (struct sockaddr_in *) servAddr;
	inetServAddr->sin_port	= htons((u_short) 9009);

	// Bind the address information to the socket.
	error = bind(listeningSocket, servAddr, sizeof(sockaddr));

	if(error == SOCKET_ERROR)
	{
		printf("error: bind() failed");
		free(servAddr);
		return -1;
	}

	free(servAddr);
	servAddr = NULL;

	// Listen for incoming connections. Queue only one connection.
	error = listen(listeningSocket, 1);

	if(error == SOCKET_ERROR)
	{
		printf("error: listen() failed");
		return -1;
	}

	// Accept the connection.
	connectedSocket = accept(listeningSocket, NULL, NULL);

	if(connectedSocket == INVALID_SOCKET)
	{
		printf("error: socket() failed");
		return -1;
	}

	return 0;
}

void ServerProcess(void)
{
	int connectionOpen;

	char buf[2];

	connectionOpen = 1;

	// Loop as long as connection is open.
	while(connectionOpen)
	{
		// Read the incoming data from the connected socket.
		if(recv(connectedSocket, buf, 2, 0))
		{
			// Set the received letter to upper-case and
			// make sure the string ends after that by setting the next
			// byte to NULL.
			buf[0] = toupper(buf[0]);
			buf[1] = '\0';

			printf("Got message from client: %s\n", buf);

			// Send the feedback.
			if(send(connectedSocket, buf, 2, 0) == SOCKET_ERROR)
			{
				connectionOpen = 0;
			}
		}
		else
		{
			closesocket(connectedSocket);
			connectionOpen = 0;
		}
	}
}

int main(void)
{
	if(dreamSock_InitializeWinSock() != 0)
	{
		printf("Critical error, quitting\n");

		return -1;
	}

	if(InitSockets() != 0)
	{
		printf("Critical error, quitting\n");

		WSACleanup();

		return -1;
	}

	ServerProcess();

	WSACleanup();

	return 0;
}