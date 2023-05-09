#include "../include/clientutils.h"

int main(int argc, char *argv[])
{
    // Parse arguments
    if (argc != 3)
        DieUsrError("Invalid number of arguments!", "IPv address port");

    char *address = argv[1];
    int ipv = checkVersion(address);
    if (ipv == 0)
        DieUsrError("Invalid address format", "Use IPv4 or IPv6 format");

    // Stablish connection
    int sock = socket(ipv, SOCK_STREAM, 0);
    if (sock < 0)
        DieSysError("failed to open socket");
    
    in_port_t port = atoi(argv[2]);
    if(ipv == 2)
    {
        struct sockaddr_in servAddr;
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = ipv;

        int rtnVal = inet_pton(ipv, address, &servAddr.sin_addr.s_addr);
        if (rtnVal == 0)
            DieUsrError("Ip string to address failed", "Invalid address string");
        else if (rtnVal < 0)
            DieSysError("Ip string to address failed");
        servAddr.sin_port = htons(port);
        if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieSysError("Connection failed");
    }else {
        struct sockaddr_in6 servAddr;
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin6_family = ipv;

        int rtnVal = inet_pton(ipv, address, &servAddr.sin6_addr);
        if (rtnVal == 0)
            DieUsrError("Ip string to address failed", "Invalid address string");
        else if (rtnVal < 0)
            DieSysError("Ip string to address failed");
            
        servAddr.sin6_port = htons(port);
        if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieSysError("Connection failed");
    }




    // Main client loop
    char *filename = NULL;
    FILE *fp;
    for (;;)
    {
        filename = cmdParse(sock);
        fp = fopen(filename, "r");
        while (sendFile(fp, filename, sock) > 0);
        fclose(fp);

        memset(filename, 0, strlen(filename));
    }
}