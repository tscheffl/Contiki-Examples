/*******************************
* Simple TCP-Server  (getaddrinfo)
* Port 1234, Print received data on screen
* Author: Thomas Scheffler
* Date: 24.11.2017
*******************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "1234"
#define MAXCHAR 1024

int main(void)
{
    int server_socket, client_socket, status;
    struct addrinfo hints;
    struct addrinfo *results;
    struct sockaddr_storage client_sa;
    socklen_t client_sa_len;
    char buff[MAXCHAR + 1];
    
    memset((void *) &hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      /* IPv4 oder IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP socket */
    hints.ai_flags = AI_PASSIVE;      /* accept connections on any IP */
    status = getaddrinfo(NULL, PORT, &hints, &results);
    if (status != 0)   
    {                                                
        fprintf(stderr, "Error in getaddrinfo: %s\n",                        
              gai_strerror(status));             
        return EXIT_FAILURE;                                    
    }                                                

    server_socket = socket(results->ai_family, 
                    results->ai_socktype,
	            results->ai_protocol);
    if (server_socket < 0)
    {
	   perror("Error in socket(2)");
	   exit(EXIT_FAILURE);
    }

    status = bind(server_socket, results->ai_addr, 
                  results->ai_addrlen);
    if (status != 0)
    {
	   perror("Error in bind(2)");
	   exit(EXIT_FAILURE);
    }
    
    status = listen(server_socket,10); /* max. 10 connections */
    if (status != 0)
    {
	   perror("Error in listen(2)");
	   exit(EXIT_FAILURE);
    }
    printf("Server listening for messages on port: %s\n", PORT);
    
    while (1)
    {    
        /* accept() creates new client socket, single threaded */
        client_socket = accept(server_socket, (struct sockaddr *) &client_sa,
                    &client_sa_len);

        /* receive data from client ... */        
        status = recv(client_socket, buff, MAXCHAR, 0);
        printf("Received %d bytes.\n",status);
        buff[status] = 0; /* Append '\0' to make it a String */

        printf("Message: %s\n",buff);
        
        close(client_socket);
    } 
}
