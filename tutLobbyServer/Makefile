#C_ARGS = -Wall
CC = gcc

all:	dreamSockServer

clean:
	rm -f *.o

dreamSockServer:	 main.o lobby.o signin.o network.o server.o
	$(CC) -o dreamSockServer main.o lobby.o signin.o network.o server.o $(C_LINK) -L. -ldreamSock -lm -lstdc++

main.o:	main.cpp
	$(CC) $(C_ARGS) -c main.cpp -L. -ldreamSock

login.o:	lobby.cpp
	$(CC) $(C_ARGS) -c lobby.cpp -L. -ldreamSock

signin.o:	signin.cpp
	$(CC) $(C_ARGS) -c signin.cpp -L. -ldreamSock

network.o:	network.cpp
	$(CC) $(C_ARGS) -c network.cpp -L. -ldreamSock

server.o:	server.cpp
	$(CC) $(C_ARGS) -c server.cpp -L. -ldreamSock -lm

