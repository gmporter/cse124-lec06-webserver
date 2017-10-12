#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include "Practical.h"

int main(int argc, char *argv[]) {

  if (argc != 2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Server Port/Service>");

  char *service = argv[1]; // First arg:  local port

  // Create socket for incoming connections
  int servSock = SetupTCPServerSocket(service);
  if (servSock < 0)
    DieWithUserMessage("SetupTCPServerSocket() failed", service);

  for (;;) { // Run forever
    // New connection creates a connected client socket
    int clntSock = AcceptTCPConnection(servSock);

    HandleTCPClient(clntSock); // Process client
    close(clntSock);
    break;
  }
  // NOT REACHED
  close(servSock);
}

void HandleTCPClient(int clntSocket) {
  char buffer[BUFSIZE]; // Buffer for incoming request
  memset(buffer, 0, BUFSIZE);

  // Receive part of the request from the client
  ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
  if (numBytesRcvd < 0)
    DieWithSystemMessage("recv() failed");

  // Send received string and receive again until end of stream
  while (numBytesRcvd > 0) { // 0 indicates end of stream
  	printf("%s", buffer);

    // See if there is more data to receive
	memset(buffer, 0, BUFSIZE);
    numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0)
      DieWithSystemMessage("recv() failed");
  }

  close(clntSocket); // Close client socket
}
