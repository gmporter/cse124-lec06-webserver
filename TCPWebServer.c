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

  // Create server socket for incoming connections

  for (;;) { // Run forever
    // Create client socket for incoming connections

    HandleTCPClient(clntSock); // Process client
  }

  // Close server socket
}

void HandleTCPClient(int clntSocket) {
	// handle the actual client socket here
}
