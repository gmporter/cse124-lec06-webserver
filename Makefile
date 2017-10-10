CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

all: client server

client: TCPWebClient.c DieWithMessage.c
	$(CC) $(CFLAGS) -o client TCPWebClient.c DieWithMessage.c

server: TCPWebServer.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -o server TCPWebServer.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf client server
