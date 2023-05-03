#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

#define MAXPENDING 5

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
        if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
            printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
        else
            puts("Unable to get client address");
    }
}