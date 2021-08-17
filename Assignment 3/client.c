#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int main(int argc, char **argv)
{
	struct addrinfo hints, *address_list, *addr;
	int error, nread;
	int sock;
	int i;
	
	if (argc < 4) {
		printf("Usage: %s [host] [port] [message(s)...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// we need to provide some additional information to getaddrinfo using hints
	// we don't know how big hints is, so we use memset to zero out all the fields
	memset(&hints, 0, sizeof(hints));
	
	// indicate that we want any kind of address
	// in practice, this means we are fine with IPv4 and IPv6 addresses
	hints.ai_family = AF_UNSPEC;
	
	// we want a socket with read/write streams, rather than datagrams
	hints.ai_socktype = SOCK_STREAM;

	// get a list of all possible ways to connect to the host
	// argv[1] - the remote host
	// argv[2] - the service (by name, or a number given as a decimal string)
	// hints   - our additional requirements
	// address_list - the list of results

	error = getaddrinfo(argv[1], argv[2], &hints, &address_list);
	if (error) {
		fprintf(stderr, "%s", gai_strerror(error));
		exit(EXIT_FAILURE);
	}

	
	// try each of the possible connection methods until we succeed
	for (addr = address_list; addr != NULL; addr = addr->ai_next) {
		// attempt to create the socket
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		
		// if we somehow failed, try the next method
		if (sock < 0) continue;
		
		// try to connect to the remote host using the socket
		if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0) {
			// we succeeded, so break out of the loop
			break;
		}

		// we weren't able to connect; close the socket and try the next method		
		close(sock);
	}
	
	// if we exited the loop without opening a socket and connecting, halt
	if (addr == NULL) {
		fprintf(stderr, "Could not connect to %s:%s\n", argv[1], argv[2]);
		exit(EXIT_FAILURE);
	}
	
	// now that we have connected, we don't need the addressinfo list, so free it
	freeaddrinfo(address_list);

    
    char bufmans[256], string_compose[256];
	char * bufmans_pointer = bufmans, * string_compose_pointer = string_compose;
	int pipe_counter = 0;

    // phase 2: receive initial + send response (Who's there?)
	memset(bufmans, 0, sizeof(bufmans)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
	while ((nread = read(sock, bufmans, 1)) > 0) { 
		bufmans[0] == '\0' ? strcpy(string_compose_pointer, bufmans_pointer) : strcat(string_compose_pointer, bufmans_pointer);

		if(strcmp(bufmans, "|") == 0) pipe_counter++;
	
		// wipes the buffer
        memset(bufmans, 0, sizeof(string_compose));

		if(pipe_counter == 3) break;
	}
	printf("Server: %s\n", string_compose);

	// send response ("Who's There?")
	
	write(sock, "R", strlen("R"));
	write(sock, "E", strlen("E"));
	write(sock, "G", strlen("G"));
	write(sock, "|", strlen("|"));
	write(sock, "1", strlen("1"));
	write(sock, "2", strlen("2"));
	write(sock, "|", strlen("|"));
	write(sock, "Who's", strlen("Who's"));
	write(sock, " there?", strlen(" there?"));
	write(sock, "|", strlen("|"));
	printf("Client: %s\n", "REG|12|Who's there?|");

	// phase 4: recieve punch line setup + respond
	memset(bufmans, 0, sizeof(bufmans)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
	pipe_counter = 0;
	while ((nread = read(sock, bufmans, 1)) > 0) { 
		bufmans[0] == '\0' ? strcpy(string_compose_pointer, bufmans_pointer) : strcat(string_compose_pointer, bufmans_pointer);

		if(strcmp(bufmans, "|") == 0) pipe_counter++;
	
		// wipes the buffer
        memset(bufmans, 0, sizeof(string_compose));

		if(pipe_counter == 3) break;
	}
	printf("Server: %s\n", "Joe.");

	write(sock, "R", strlen("R"));
	write(sock, "E", strlen("E"));
	write(sock, "G", strlen("G"));
	write(sock, "|", strlen("|"));
	write(sock, "9", strlen("9"));
	write(sock, "|Joe", strlen("|Joe"));
	write(sock, ", who?|", strlen(", who?|"));

	// phase 6: receive punch line + rage
	memset(bufmans, 0, sizeof(bufmans)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
	pipe_counter = 0;
	while ((nread = read(sock, bufmans, 1)) > 0) { 
		bufmans[0] == '\0' ? strcpy(string_compose_pointer, bufmans_pointer) : strcat(string_compose_pointer, bufmans_pointer);

		if(strcmp(bufmans, "|") == 0) pipe_counter++;
	
		// wipes the buffer
        memset(bufmans, 0, sizeof(string_compose));

		if(pipe_counter == 3) break;
	}
	printf("Server: %s\n", string_compose_pointer);

	write(sock, "R", strlen("R"));
	write(sock, "E", strlen("E"));
	write(sock, "G", strlen("G"));
	write(sock, "|8|", strlen("|8|"));
	write(sock, "wdf man", strlen("wdf man"));
	write(sock, ".|", strlen(".|"));
	printf("Client: %s\n", "wdf man");

	


    
	
	  // close the socket
	  close(sock);

	  return EXIT_SUCCESS;  

}
