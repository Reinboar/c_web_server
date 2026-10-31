#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "http_prot.h"
#include "req_handler.h"

#define LISTEN_PORT 8080
#define CLIENT_BUFFER_SIZE 1024*1024*2 // 2MB per client buffer

void signalHandler(int signal) {
    
}

int createListenSocket() {
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(LISTEN_PORT);
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    int activate = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &activate, sizeof(activate));

    if (bind(serverFd, (void*)&sa, sizeof(sa)) != 0) {
        printf("Error while binding socket.\n");
        exit(-1);
    }

    return serverFd;
}

int acceptClientConnection(int serverFd, struct sockaddr_in* ca) {
    unsigned int address_size;
    return accept(serverFd,(void*)ca, &address_size);
}

int clientSocketProcess(int clientFd) {
    char* buffer = malloc(CLIENT_BUFFER_SIZE); // input buffer
    int inSize; // size of the received message

    while (1) {
        bzero(buffer, CLIENT_BUFFER_SIZE);
        int actualSize = recv(clientFd, buffer, CLIENT_BUFFER_SIZE-1, 0); 
        if (actualSize >= CLIENT_BUFFER_SIZE) {
            printf("Request was longer than the permitted size of %dMB.\n", CLIENT_BUFFER_SIZE/(1024*1024));
            break; // kill current process
        }
        HttpRequest* req = readHttpReq(&buffer);
        processRequest(req, clientFd);
        destroyHttpReq(req);
    }
}

int main() {
    pid_t child_pid;

    int serverFd = createListenSocket();

    // Listen for incoming connections
    if (listen(serverFd, 100)!=0) {
        printf("Error occurred while attempting to listen on socket.\n");
        exit(-1);
    }

    printf("Listening...\n");

    signal(SIGUSR1, signalHandler);

    while(1) {
        // Accept an incoming connection
        struct sockaddr_in ca;
        int clientFd = acceptClientConnection(serverFd, &ca);

        // If error occurred while creating client socket error out
        if (clientFd < 0) {
            printf("Could not create client socket.\n");
            exit(-1);
        }
        
        // Display the client connection information
        printf("<- %s:%d\n", inet_ntoa(ca.sin_addr), ntohs(ca.sin_port));

        // Fork a process to handle data from the client connection
        if (child_pid = fork() == 0) {
            clientSocketProcess(clientFd);
            return 0;
        }
    }

}