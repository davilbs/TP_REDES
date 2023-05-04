#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

#define MAXPENDING 5

void handleClient(int clntSocket)
{
    char buffer[BUFFERSIZE];
    char header[BUFFERSIZE];
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFERSIZE, 0);
    if(numBytesRcvd < 0)
        DieSysError("Failed to receive message");
    
    while(numBytesRcvd > 0)
    {
        int pos = 0;
        for(;;pos++)
        {
            if(buffer[pos] == '\\' && buffer[pos+1] == 'e' && buffer[pos+2] == 'n' && buffer[pos+3] == 'd')
                break;
        }

        strncat(header, buffer, pos);
        printf("Pos %d - %s\n", pos, header);
        if (memcmp(header, "exit", sizeof("exit")) == 0)
        {
            size_t strLen = strlen("connection closed\\end");
            ssize_t numBytes = send(clntSocket, "connection closed\\end", strLen, 0);
            break;
        }

        ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFERSIZE, 0);
        if(numBytesRcvd < 0)
            DieSysError("Failed to receive message");
    }
    close(clntSocket);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        DieUsrError("Invalid number of arguments", "Ipv Port");
    
    char* version = argv[1];
    in_port_t port = atoi(argv[2]);
    printf("IPversion [port]: %s [%d]\n", version, port);

    int servSock;
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieSysError("Failed to open socket");
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieSysError("Failed to bind to socket");
    
    if (listen(servSock, MAXPENDING) < 0)
        DieSysError("Failed to listen on port");
    
    for(;;) {
        struct sockaddr_in clntAddr;
        socklen_t clntAddrLen = sizeof(clntAddr);

        int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
        if(clntSock < 0)
            DieSysError("Failed to accept connection");
        
        char clntName[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL){
            printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
            handleClient(clntSock);
        }
        else
            puts("Unable to get client address");
        printf("Finished handling client\n");
    }
}