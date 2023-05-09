#include "common.h"

#define MAXPENDING 5

// Writes the content from the buffer to the file
void writeContent(FILE *fp, const char *content)
{
    for (int pos = 0;; pos++)
    {
        if (content[pos] == '\0')
            break;
        fputc(content[pos], fp);
    }
}

int processText(FILE *fp, const char *filename, const char *content, int clntSocket, int isFile)
{
    if (strlen(content) == 0)
    {
        printf("File status: %d\n", isFile);
        fclose(fp);
        char serverMsg[BUFFERSIZE];
        memset(serverMsg, 0, BUFFERSIZE);
        strcat(serverMsg, "file ");
        strcat(serverMsg, filename);
        strcat(serverMsg, (isFile == 1) ? " received\\end" : " overwritten\\end");
        size_t strLen = strlen(serverMsg);
        ssize_t numBytes = send(clntSocket, serverMsg, strLen, 0);
        puts("Finished writing");
        return 0;
    } 
    size_t strLen = strlen("received\\end");
    ssize_t numBytes = send(clntSocket, "received\\end", strLen, 0);

    writeContent(fp, content);
    return isFile;
}

// Handles a client connection until it is terminated by the client
void handleClient(int clntSocket)
{
    char buffer[BUFFERSIZE];
    char *filename = NULL;
    char *header = NULL;
    char *content = NULL;
    FILE *fp;
    int isFile = 0;
    for (;;)
    {
        ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFFERSIZE, 0);
        if (numBytesRcvd < 8)
            DieSysError("Failed to receive message");

        // Check header for exit command
        header = parseMsg(buffer, 0);
        if (strcmp(header, "exit") == 0)
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
            filename = (char *) malloc(strlen(header));
            strcpy(filename, header);
            isFile = (access(filename, F_OK) != -1) ? 2 : 1;
            fp = fopen(filename, "w");
            printf("Setting file status: %d\n", isFile);
        }
        isFile = processText(fp, filename, content, clntSocket, isFile);

        memset(buffer, 0, BUFFERSIZE);
        memset(header, 0, strlen(header));
        memset(content, 0, strlen(content));
    }
}