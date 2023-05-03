#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

#define BUFFERSIZE 500
const char *formats[] = {"txt", "c", "cpp", "py", "tex", "java"};

int checkFormat(const char *format)
{
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(format, formats[i]) == 0)
            return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        DieUsrError("Invalid number of arguments!", "IPv address port");
    char *address = argv[1];
    in_port_t port = atoi(argv[2]);
    printf("Server address [port]: %s [%d]\n", address, port);

    // Stablish connection
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock < 0)
        DieSysError("failed to open socket");

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;

    int rtnVal = inet_pton(AF_INET, address, &servAddr.sin_addr.s_addr);
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
    char *format = "";
    int isSend;
    size_t inputSize;
    char filecontent[BUFFERSIZE];
    FILE *fp;
    for (;;)
    {
        isSend = 0;
        while (!isSend)
        {
            if (getline(&input, &inputSize, stdin) < 0)
                exit(1);
            command = strtok(input, " \n");
            if (memcmp(command, "exit", sizeof("exit")) == 0)
            {
                // Close connection
                close(sock);
                exit(0);
            }
            else if (memcmp(command, "select", sizeof("select")) == 0)
            {
                // Get filename
                strtok(NULL, " \n");
                filename = strtok(NULL, " \n");
                fp = fopen(filename, "r");
                strtok(filename, ".");
                format = strtok(NULL, ".");
                // strcpy(format, filename);
                if (fp == NULL)
                {
                    printf("%s.%s does not exist\n", filename, format);
                    filename = "";
                }
                else if (checkFormat(format) == 0)
                {
                    printf("%s.%s not valid!\n", filename, format);
                    filename = "";
                }
            }
            else if (memcmp(command, "send", sizeof("send")) == 0)
            {
                if (filename == "")
                    printf("no file selected!\n");
                else
                    isSend = 1;
            }
        }

        printf("%s selected\n", filename);
        size_t readLen;
        do
        {
            readLen = fread(filecontent, sizeof(char), BUFFERSIZE - 4, fp);
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
        } while (readLen == (BUFFERSIZE - 4));
        fclose(fp);
    }
}