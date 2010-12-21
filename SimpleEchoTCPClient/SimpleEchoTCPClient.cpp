/*
	Simple echo TCP client
	Teijo Hakala 2001
*/

// Define _WINSOCKAPI_ so windows.h will not include old WinSock header.
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <windows.h>
#include <winsock2.h>

#include <stdio.h>

// Declare the only socket we need.
SOCKET Socket;

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

int InitSockets(char *IPaddress)
{
	struct sockaddr_in inetServAddr;
	int error = 0;

	// Create a TCP socket.
	Socket = socket(AF_INET, SOCK_STREAM, 0);

	if(Socket < 0)
	{
		printf("error: socket() failed");
		return -1;
	}

	// Create the Internet address from the IP number
	u_long inetAddr = inet_addr(IPaddress);

	memset((char *) &inetServAddr, 0, sizeof(inetServAddr));
	inetServAddr.sin_family			= AF_INET;
	inetServAddr.sin_port			= htons((u_short) 9009);
	inetServAddr.sin_addr.s_addr	= inetAddr;

	// Try to connect the TCP server.
	error = connect(Socket, (struct sockaddr *) &inetServAddr, sizeof(inetServAddr));

	if(error != 0)
	{
		printf("error: could not find server.\n");
		return -1;
	}

	return 0;
}

void ClientProcess(void)
{
	int connectionOpen;

	char transmitBuf[3];
	char receiveBuf[3];
	strcpy(transmitBuf, "");
	strcpy(receiveBuf, "");

	connectionOpen = 1;

	// Loop as long as connection is open.
	while(connectionOpen)
	{
		// Get the string to send.
		if(gets(transmitBuf))
		{
			if(strcmp(transmitBuf, "q") == 0)
			{
				closesocket(Socket);
				connectionOpen = 0;
				break;
			}

			// Send the transmit buffer to the socket.
			if(send(Socket, transmitBuf, 2, 0) == SOCKET_ERROR)
			{
				connectionOpen = 0;
			}
		}

		// Read the incoming data from the connected socket.
		if(recv(Socket, receiveBuf, 2, 0))
		{
			printf("Got reply from server: %s\n", receiveBuf);
		}
		else
		{
			connectionOpen = 0;
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: SimpleEchoTCPClient.exe <Server IP>\n");
		return -1;
	}

	dreamSock_InitializeWinSock();

	if(InitSockets(argv[1]) != 0)
	{
		printf("Critical error, quitting\n");

		WSACleanup();

		return -1;
	}

	ClientProcess();

	WSACleanup();

	return 0;
}