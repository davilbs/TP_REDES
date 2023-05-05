#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

void writeContent(FILE *fp, char *content)
{
    int inContent = 0;
    for (int pos = 0;; pos++)
    {
        if (checkEnd(content, pos))
        {
            if (inContent)
                break;
            else
                inContent = 1;
        }
        else if (inContent)
        {
            fputc(content[pos], fp);
        }
    }
}

void handleClient(int clntSocket)
{
    char buffer[BUFFERSIZE];
    char serverMsg[BUFFERSIZE];
    char *filename;
    int isFile = 0;
    FILE *fp;
    for (;;)
    {
        ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFERSIZE, 0);

        if (numBytesRcvd < 0)
        {
            DieSysError("Failed to receive message");
        }
        else if ((numBytesRcvd == 0) && isFile)
        {
            strcat(serverMsg, "file ");
            strcat(serverMsg, filename);
            strcat(serverMsg, (isFile == 1) ? " received\\end" : " overwritten\\end");
            size_t strLen = strlen(serverMsg);
            ssize_t numBytes = send(clntSocket, serverMsg, strLen, 0);
            isFile = 0;
        }
        else
        {
            char *header = parseMsg(buffer);
            if (memcmp(header, "exit", sizeof("exit")) == 0)
            {
                size_t strLen = strlen("connection closed\\end");
                ssize_t numBytes = send(clntSocket, "connection closed\\end", strLen, 0);
                close(clntSocket);
                return;
            }
            else
            {
                if (!isFile)
                {
                    isFile = (access(header, F_OK) == 0) ? 2 : 1;
                    fp = fopen(header, "w");
                    filename = (char *)malloc(strlen(header) + 1);
                    strcpy(filename, header);
                }
                writeContent(fp, buffer);
            }
        }
    }
}