#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#define BACKLOG 5


struct connection {
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

int server(char *port);
void *echo(void *arg);

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    (void) server(argv[1]);
    return EXIT_SUCCESS;
}


int server(char *port)
{
    struct addrinfo hint, *address_list, *addr;
    struct connection *con;
    int error, sfd;
    pthread_t tid;

    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    	// setting AI_PASSIVE means that we want to create a listening socket

    // get socket and address info for listening port
    // - for a listening socket, give NULL as the host name (because the socket is on
    //   the local host)
    error = getaddrinfo(NULL, port, &hint, &address_list);
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (addr = address_list; addr != NULL; addr = addr->ai_next) {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        
        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        // 
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, addr->ai_addr, addr->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (addr == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(address_list);

    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
    for (;;) {
    	// create argument struct for child thread
		con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
        	// addr_len is a read/write parameter to accept
        	// we set the initial value, saying how much space is available
        	// after the call to accept, this field will contain the actual address length
        
        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
        	// we provide
        	// sfd - the listening socket
        	// &con->addr - a location to write the address of the remote host
        	// &con->addr_len - a location to write the length of the address
        	//
        	// accept will block until a remote host tries to connect
        	// it returns a new socket that can be used to communicate with the remote
        	// host, and writes the address of the remote hist into the provided location
        
        // if we got back -1, it means something went wrong
        if (con->fd == -1) {
            perror("accept");
            continue;
        }

		// spin off a worker thread to handle the remote connection
        error = pthread_create(&tid, NULL, echo, con);

		// if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0) {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

		// otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);
    }

    // never reach here
    return 0;
}

// token = the regular message
// level = which message we are checking
int tokenize (char *str, int level, struct connection *c) {

    // level 3 tokenizer (REG|12|Who's There|)
    if(level == 3) {
        if(strcmp("REG|12|Who's there?|", str) == 0) return 0; // 0 means correct message

        // setup for the error return
        char *token = strtok(str, "|");
        int count = 0;
        int length;

        if(token == NULL) return 6; // format error

        while(token != NULL) {
            if (count == 0) {
                if(strcmp(token, "REG") != 0) return 6; // format error
            } 
            
            if(count == 1) {
                if(atoi(token) == 0) return 6; // format error
                length = atoi(token);
            }

            if(count == 2) {
                if(length != strlen(token)) return 5; // length error
                if(strcmp(token, "Who's there?") != 0) return 4; // message error
            }

            count++;
            token = strtok(NULL , "|");
        }
    }

    // level 5 tokenizer (REG|9|Joe, who?|)
    if(level == 5) {

        if(strcmp("REG|9|Joe, who?|", str) == 0) return 0; // correct message

        // setup for the error return
        char *token = strtok(str, "|");
        int count = 0;
        int length;


        if(token == NULL) return 12; // format error
        while(token != NULL) {
            if (count == 0) {
                if(strcmp(token, "REG") != 0) return 12; // format error
            } 
            
            if(count == 1) {
                if(atoi(token) == 0) return 12; // format error
                length = atoi(token);
            }

            if(count == 2) {
                if(length != strlen(token)) return 11; // length error
                if(strcmp(token, "Joe, who?") != 0) return 10; // message conent
            }

            count++;
            token = strtok(NULL , "|");
        }
    }

    // level 7 tokenizer (REG | LENGTH | STRING) 
    if(level == 7) {
        char * token = strtok(str, "|");
        int count = 0, length = 0;

        if(token == NULL) return 18; // format error

        while(token != NULL) {
            if(count == 0) {
                if(strcmp(token, "REG") != 0) return 18; // format error
            }

            if(count == 1) length = atoi(token);

            if(count == 2) break;

            count ++;
            token = strtok(NULL, "|");
        }

        if(length == 0) return 18; // format error
        if(length != strlen(token)) return 17; // lentgh error

        if(token[strlen(token) - 1] == '.' || token[strlen(token) - 1] == '?' || token[strlen(token) - 1] == '!') { // message content error
            return 0;
        } else {
            return 16;
        }
    }
    return 0;
}

void *echo(void *arg)
{
    char host[100], port[10];
    char * string_compose = malloc(sizeof(char) * 4096), * buffer = malloc(sizeof(char) * 4096);
    char * string_compose_pointer = string_compose, * buffer_pointer = buffer;
    struct connection *c = (struct connection *) arg;
    int error, nread, stat, level = 1, index = 0, eval;

	// find out the name and port of the remote host
    error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
    	// we provide:
    	// the address and its length
    	// a buffer to write the host name, and its length
    	// a buffer to write the port (as a string), and its length
    	// flags, in this case saying that we want the port as a number, not a service name
    if (error != 0) {
        fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
        close(c->fd);
        return NULL;
    }

    printf("[%s:%s] Server is Online.\n", host, port);

    // start of knock knock joke | this is the code without accounting for multiple reads
    // phase 1: receive client connection + send initial to client
    write(c->fd, "REG|13|Knock, knock.|", strlen("REG|13|Knock, knock.|"));

    // phase 3: recv response + send setup to client: (should be receiving Who's There?)
    memset(buffer, 0, sizeof(buffer)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
    int pipe_counter = 0; // keeps track of the pipes
    level = 3; // keeps track of the phase so we know for tokenizer

    // loop to read "Who's There"
    while ((nread = read(c->fd, buffer, 1)) > 0 || pipe_counter != 3) {   

        // appends buffer to string compose
        buffer[0] == '\0' ? strcpy(string_compose_pointer, buffer_pointer) : strcat(string_compose_pointer, buffer_pointer);

        if(strcmp(buffer, "|") == 0) pipe_counter++;
        
        // wipes the buffer
        memset(buffer, 0, sizeof(buffer));

        // can stop reading because message is done
        if(pipe_counter == 3) break;

        // client sends us error (should never happen unless a artifical error is sent)
        if(strcmp(string_compose_pointer, "ERR|M0CT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M0CT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M0LN|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M0LN|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M0FT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M0FT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }

        // condition where the client ends the connection to the server
         if(nread == 0) {
            printf("[%s:%s] Client has disconnected before completely writing to server.\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }
    }

    // value of tokenize
    eval = tokenize(string_compose_pointer, level, c);
    if(eval == 4) {
        write(c->fd, "ERR|M1CT|", strlen("ERR|M1CT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M1CT\n", host, port);
    }
    if(eval == 5) {
        write(c->fd, "ERR|M1LN|", strlen("ERR|M1LN|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M1LN\n", host, port);
    }
    if(eval == 6) {
        write(c->fd, "ERR|M1FT|", strlen("ERR|M1FT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M1FT\n", host, port);
    }

    // if error case, close it out
    if(eval == 4 || eval == 5 || eval == 6) {
        close(c->fd);
        free(c);
        return NULL;
    }
    
    // writing Joe.
    write(c->fd, "REG|4|Joe.|", strlen("REG|4|Joe.|"));

    // end of phase 3.

    // phase 5: receive punch line response + send punchline (Joe, who?)
    memset(buffer, 0, sizeof(buffer)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
    pipe_counter = 0;
    level = 5;

    while ((nread = read(c->fd, buffer, 1)) > 0 || pipe_counter != 3) {       
        // appends buffer to string compose
        buffer[0] == '\0' ? strcpy(string_compose_pointer, buffer_pointer) : strcat(string_compose_pointer, buffer_pointer);

        if(strcmp(buffer, "|") == 0) pipe_counter++;
        
        // wipes the buffer
        memset(buffer, 0, sizeof(buffer));

        if(pipe_counter == 3) break;

        // should never occur because accounts for the fact if the client decides that the output of a server is errorenous
        // client will close and free the socket then return back to the infinite loop
        if(strcmp(string_compose_pointer, "ERR|M2CT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M2CT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M2LN|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M2LN|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M2FT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M2FT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }

        // break condition for the client
        if(nread == 0) {
            printf("[%s:%s] Client has disconnected before completely writing to server.\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }


    }
    // tokenizer that decides what it will do after recieving the message
    eval = tokenize(string_compose_pointer, level, c);

    // error cases. 12 = M3FT | 11 = M3LN | 10 = M3CT
    if(eval == 12) {
        write(c->fd, "ERR|M3FT|", strlen("ERR|M3FT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M3FT\n", host, port);
    }
    if(eval == 11) {
        write(c->fd, "ERR|M3LN|", strlen("ERR|M3LN|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M3LN\n", host, port);
    }
    if(eval == 10) {
        write(c->fd, "ERR|M3CT|", strlen("ERR|M3CT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M3CT\n", host, port);
    }

    // if error case, close it out
    if(eval == 12 || eval == 11 || eval == 10) {
        close(c->fd);
        free(c);
        return NULL;
    }

    write(c->fd, "REG|9|Joe Mama!|", strlen("REG|9|Joe Mama!|"));

    // phase 7: receive and end
    memset(buffer, 0, sizeof(buffer)); // makes sure that the buffer is empty
    memset(string_compose, 0, sizeof(string_compose)); // makes sure that the string we're building is empty
    pipe_counter = 0;
    level = 7;

    while ((nread = read(c->fd, buffer, 1)) > 0 || pipe_counter != 3) {   

        // appends buffer to string compose
        buffer[0] == '\0' ? strcpy(string_compose_pointer, buffer_pointer) : strcat(string_compose_pointer, buffer_pointer);

        if(strcmp(buffer, "|") == 0) pipe_counter++;
        
        // wipes the buffer
        memset(buffer, 0, sizeof(buffer));

        if(pipe_counter == 3) break;

        if(strcmp(string_compose_pointer, "ERR|M4CT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M4CT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M4LN|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M4LN|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        } else if(strcmp(string_compose_pointer, "ERR|M4FT|") == 0 && pipe_counter == 2) {
            printf("[%s:%s] Client has disconnected because the server recieved an error. CODE: ERR|M4FT|\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }

        // case for when client closes the connection to server before completely writing
        if(nread == 0) {
            printf("[%s:%s] Client has disconnected before completely writing to server.\n", host, port);
            close(c->fd);
            free(c);
            return NULL;
        }

    }

    // tokenize the character buffer and decide what to do with it
    eval = tokenize(string_compose_pointer, level, c);

    // error conditions | 18 = M5FT | 17 = M5LN | 16 M5CT
    if(eval == 18) {
        write(c->fd, "ERR|M5FT|", strlen("ERR|M5FT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M5FT\n", host, port);
    }
    if(eval == 17) {
        write(c->fd, "ERR|M5LN|", strlen("ERR|M5LN|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M5LN\n", host, port);
    }    
    if(eval == 16) {
        write(c->fd, "ERR|M5CT|", strlen("ERR|M5CT|"));
        printf("[%s:%s] Client socket is now being disconnected. Error Code: M5CT\n", host, port);
    }

    // if the error conditions are triggered, since we already printed the error conditions, we can close the connection
    if(eval == 16 || eval == 17 || eval == 18) {
        close(c->fd);
        free(c);
        return NULL;
    }

    printf("[%s:%s] Function has successfully executed. Client socket is now being disconnected.\n", host, port);;

    close(c->fd);
    free(c);
    return NULL;
}
