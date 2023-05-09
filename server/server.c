#include "../include/serverutils.h"

int main(int argc, char *argv[])
{
    // Parse arguments
    if (argc != 3)
        DieUsrError("Invalid number of arguments", "Ipv Port");

    char *version = argv[1];
    in_port_t port = atoi(argv[2]);

    // Check IP version
    // Open network socket
    int servSock;
    if (strcmp(version, "v4") == 0)
    {
        servSock = socket(AF_INET, SOCK_STREAM, 0);
        if (servSock < 0)
            DieSysError("Failed to open socket");

        struct sockaddr_in servAddr;
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);

        if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieSysError("Failed to bind to socket");
    }
    else if (strcmp(version, "v6") == 0)
    {
        servSock = socket(AF_INET6, SOCK_STREAM, 0);
        if (servSock < 0)
            DieSysError("Failed to open socket");

        struct sockaddr_in6 servAddr;
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin6_family = AF_INET6;
        servAddr.sin6_addr = in6addr_any;
        servAddr.sin6_port = htons(port);

        if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieSysError("Failed to bind to socket");
    }
    else
        DieUsrError("Invalid IP version", "use v4 or v6");

    if (listen(servSock, MAXPENDING) < 0)
        DieSysError("Failed to listen on port");

    // Main server loop
    if (strcmp(version, "v4") == 0)
    {
        for (;;)
        {
            struct sockaddr_in clntAddr;
            socklen_t clntAddrLen = sizeof(clntAddr);

            int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
            if (clntSock < 0)
                DieSysError("Failed to accept connection");

            char clntName[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
                handleClient(clntSock);
            else
                puts("Unable to get client address");
        }
    }
    else
    {
        for (;;)
        {
            struct sockaddr_in6 clntAddr;
            socklen_t clntAddrLen = sizeof(clntAddr);

            int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
            if (clntSock < 0)
                DieSysError("Failed to accept connection");

            char clntName[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET6, &clntAddr.sin6_addr, clntName, sizeof(clntName)) != NULL)
                handleClient(clntSock);
            else
                puts("Unable to get client address");
        }
    }

    close(servSock);
    exit(0);
}