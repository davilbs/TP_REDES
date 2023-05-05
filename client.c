#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

const char *formats[] = {".txt", ".c", ".cpp", ".py", ".tex", ".java"};

int checkVersion(const char *address)
{
    int pos = 0;
    while (address[pos] != '\0')
    {
        printf("%c", address[pos]);
        if (address[pos] == '.')
            return 2;
        if (address[pos] == ':')
            return 10;
        pos++;
    }
    return 0;
}

int checkFormat(const char *filename)
{
    char* format = strrchr(filename, '.');
    if (format == NULL) return 0;
    printf("File format: %s\n", format);
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(format, formats[i]) == 0)
            return 1;
    }
    return 0;
}

size_t readFile()
{
    char filecontent[BUFFERSIZE];
    size_t readLen;
    readLen = fread(filecontent, sizeof(char), BUFFERSIZE - 4 - inputSize, fp);
    if (ferror(fp) != 0)
        DieSysError("Error reading file");
    if (readLen > 0)
    {
        printf("[%ld] %s\n", readLen, filecontent);
        filecontent[readLen++] = '\\';
        filecontent[readLen + 2] = 'e';
        filecontent[readLen + 3] = 'n';
        filecontent[readLen + 4] = 'd';
    }
    printf("Read %lu bytes\n", readLen);
    memset(filecontent, 0, BUFFERSIZE);
    printf("Reset buffer %s\n", filecontent);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        DieUsrError("Invalid number of arguments!", "IPv address port");
    char *address = argv[1];
    int ipv = checkVersion(address);
    int proto = (ipv == 2) ? 0 : 41;
    in_port_t port = atoi(argv[2]);
    printf("Server address [port]: %s [%d] ", address, port);

    if (ipv == 0)
        DieUsrError("Invalid address format", "Use IPv4 or IPv6 format");
    else if (ipv == 2)
        printf("IPv4\n");
    else
        printf("IPv6\n");

    // Stablish connection
    int sock = socket(ipv, SOCK_STREAM, proto);

    if (sock < 0)
        DieSysError("failed to open socket");

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

    // Get commands
    char *input = NULL;
    char *command;
    char *filename = "";
    char msg[BUFFERSIZE];
    size_t inputSize;
    FILE *fp;
    printf("Input loop\n");
    for (;;)
    {
        for (;;)
        {
            if (getline(&input, &inputSize, stdin) < 0)
                exit(1);

            command = strtok(input, " \n");
            if (memcmp(command, "exit", sizeof("exit")) == 0)
            {
                // Close connection
                size_t strLen = strlen("exit\\end");
                ssize_t numBytes = send(sock, "exit\\end", strLen, 0);

                if (numBytes < 0)
                    DieSysError("Failed to send exit command");
                else if(numBytes != strLen)
                    DieUsrError("Failed to send", "Sent unexpected number of bytes");
                    
                ssize_t numBytesRcvd = recv(sock, msg, BUFFERSIZE, 0);
                if(numBytesRcvd < 0)
                    DieSysError("Failed to receive message");

                printf("%s\n", parseMsg(msg));
                close(sock);
                exit(0);
            }
            else if (memcmp(command, "select", sizeof("select")) == 0)
            {
                // Get filename
                strtok(NULL, " \n");
                filename = strtok(NULL, " \n");
                if (access(filename, F_OK) != 0)
                {
                    printf("%s does not exist\n", filename);
                    filename = "";
                }
                else if (checkFormat(filename) == 0)
                {
                    printf("%s not valid!\n", filename);
                    filename = "";
                }
            }
            else if (memcmp(command, "send", sizeof("send")) == 0)
            {
                if (filename == "")
                    printf("no file selected!\n");
                else
                    break;
            }
        }

        fp = fopen(filename, "r");
        msg = 
        while ( > 0);
        fclose(fp);
        printf("Finished reading file\n");
    }
}