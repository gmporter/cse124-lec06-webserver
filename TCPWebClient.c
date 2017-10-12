#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

/* Given a url, return the server hostname/domain-name portion */
void extractServerFromURL(const char * const url, char * buf, int buflen);

/* Given a url, return the query part (after the server portion) */
void extractQueryFromURL(const char * const url, char * buf, int buflen);

/* Given a sockaddr struct, return it as a string (from D&C book) */
char * get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

int main(int argc, char *argv[]) {
  char *url = argv[1];
  char serverStr[100];
  char queryStr[1024];
  struct addrinfo hints, *servinfo, *p;
  int rv, sock;

  memset(serverStr, 0, 100);
  memset(queryStr, 0, 1024);

  if (argc != 2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<URL>");

  extractServerFromURL(url, serverStr, 80);
  extractQueryFromURL(url, queryStr, 1024);

  fprintf(stderr, "Connecting to %s to request %s\n", serverStr, queryStr);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     // either IPv6 or IPV4 OK
  hints.ai_socktype = SOCK_STREAM; // use tcp

  if ((rv = getaddrinfo(serverStr, "http", &hints, &servinfo)) != 0) {
  	DieWithUserMessage("getaddrinfo", gai_strerror(rv));
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
  	if ((sock = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
		DieWithUserMessage("socket", "Couldn't connect to address");
	}

	if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
		DieWithUserMessage("socket", "can't connect");
	}

	break;
  }

  if (p == NULL) {
  	DieWithUserMessage("socket", "Didn't find an address to connect to");
  }
  freeaddrinfo(servinfo);

  char addrbuf[128];
  get_ip_str(p->ai_addr, addrbuf, 128);
  fprintf(stderr, "Connected to %s!\n", addrbuf);

  // Send the query to the server
  char query[1024];
  snprintf(query, 1024, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",
		queryStr, serverStr);
  fputs(query, stderr);

  ssize_t numBytes = send(sock, query, strlen(query), 0);
  if (numBytes < 0)
    DieWithSystemMessage("send() failed");

  fprintf(stderr, "Send the query!\n");

  // Receive the web response
  do {
    char buffer[BUFSIZE]; // I/O buffer
    numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
    if (numBytes < 0)
      DieWithSystemMessage("recv() failed");
    else if (numBytes == 0)
      break;
    buffer[numBytes] = '\0';    // Terminate the string!
    fputs(buffer, stdout);      // Print the echo buffer
  } while (numBytes > 0);

  fputc('\n', stdout); // Print a final linefeed

  close(sock);
  fprintf(stderr, "Closed the socket!\n");

  exit(0);
}

void
extractServerFromURL(const char * const url, char * buf, int buflen)
{
	const char * PREAMBLE = "http://";
	const int PREAMBLE_LEN = strlen(PREAMBLE);

	if (strncmp(PREAMBLE, url, PREAMBLE_LEN)) {
		DieWithUserMessage("badurl", "No http:// preamble");
	}

	const char * server = url+PREAMBLE_LEN;
	const char * slashptr = strstr(server, "/");

	if (!slashptr) {
		DieWithUserMessage("badurl", "No / after the server address");
	}

	if ((slashptr - server) > buflen) {
		DieWithUserMessage("badurl", "buf too small");
	}
	assert(slashptr - server < buflen);

	strncpy(buf, server, (slashptr - server));
}

void
extractQueryFromURL(const char * const url, char * buf, int buflen)
{
	const char * PREAMBLE = "http://";
	const int PREAMBLE_LEN = strlen(PREAMBLE);

	if (strncmp(PREAMBLE, url, PREAMBLE_LEN)) {
		DieWithUserMessage("badurl", "No http:// preamble");
	}

	const char * server = url+PREAMBLE_LEN;
	const char * slashptr = strstr(server, "/");

	if (!slashptr) {
		DieWithUserMessage("badurl", "No / after the server address");
	}

	strcpy(buf, slashptr);
}

char * get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
	switch(sa->sa_family) {
		case AF_INET:
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
				s, maxlen);
				break;

		case AF_INET6:
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
				s, maxlen);
				break;

		default:
			strncpy(s, "Unknown AF", maxlen);
			return NULL;
	}

	return s;
}
