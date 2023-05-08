#include "common.h"

#define MAXPENDING 5

// Writes the content from the buffer to the file
void writeContent(FILE *fp, const char *content)
{
    printf("Writing [%s] to file\n", content);
    for (int pos = 0;; pos++)
    {
        printf("%c", content[pos]);
        if (content[pos] == '\0')
            break;
        fputc(content[pos], fp);
    }
    puts(" Written");
}

int processText(FILE *fp, const char *filename, const char *content, int clntSocket, int isFile)
{
    if (strlen(content) == 0)
    {
        fclose(fp);
        puts("Finished writing");
        char serverMsg[BUFFERSIZE];
        strcat(serverMsg, "file ");
        strcat(serverMsg, filename);
        strcat(serverMsg, (isFile == 1) ? " received\\end" : " overwritten\\end");
        size_t strLen = strlen(serverMsg);
        ssize_t numBytes = send(clntSocket, serverMsg, strLen, 0);
        return 0;
    }

    writeContent(fp, content);
    return isFile;
}

// Handles a client connection until it is terminated by the client
void handleClient(int clntSocket)
{
    char buffer[BUFFERSIZE];
    char *filename;
    char *header;
    char *content;
    FILE *fp;
    int isFile = 0;
    for (;;)
    {
        ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFERSIZE, 0);
        printf("Message received [%s]\n", buffer);
        if (numBytesRcvd < 8)
            DieSysError("Failed to receive message");

        // Check header for exit command
        header = parseMsg(buffer, 0);
        if (memcmp(header, "exit", sizeof("exit")) == 0)
        {
            size_t strLen = strlen("connection closed\\end");
            ssize_t numBytes = send(clntSocket, "connection closed\\end", strLen, 0);
            close(clntSocket);
            return;
        }

        // Check received content
        content = parseMsg(buffer, strlen(header) + 4);
        if (!isFile)
        {
            fp = fopen(header, "w");
            filename = (char *)malloc(strlen(header));
            strcpy(filename, header);
            isFile = (access(filename, F_OK) == 0) ? 2 : 1;
            processText(fp, filename, content, clntSocket, isFile);
        }
        else
        {
            isFile = processText(fp, filename, content, clntSocket, isFile);
        }

        memset(buffer, 0, BUFFERSIZE);
    }
}