#include "common.h"

const char *formats[] = {".txt", ".c", ".cpp", ".py", ".tex", ".java"};

// Checks the IP version for the connection
int checkVersion(const char *address)
{
    int pos = 0;
    while (address[pos] != '\0')
    {
        if (address[pos] == '.')
            return 2;
        if (address[pos] == ':')
            return 10;
        pos++;
    }
    return 0;
}

// Checks the file format is in the approved list
int checkFormat(const char *filename)
{
    const char *format = strrchr(filename, '.');
    if (format == NULL)
        return 0;

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(format, formats[i]) == 0)
            return 1;
    }
    return 0;
}

// Checks if the file can be sent
int checkFile(const char *filename)
{
    int ok = 1;
    if (access(filename, F_OK) != 0)
    {
        printf("%s does not exist\n", filename);
        ok = 0;
    }
    else if (checkFormat(filename) == 0)
    {
        printf("%s not valid!\n", filename);
        ok = 0;
    }else {
        printf("%s selected\n", filename);
    }
    return ok;
}

// Closes the connection with the server
void sendExit(int sock)
{
    char msg[BUFFERSIZE];
    size_t strLen = strlen("exit\\end");
    ssize_t numBytes = send(sock, "exit\\end", strLen, 0);

    if (numBytes < 0)
        DieSysError("Failed to send exit command");
    else if (numBytes != strLen)
        DieUsrError("Failed to send", "Sent unexpected number of bytes");

    ssize_t numBytesRcvd;
    do{
        numBytesRcvd = recv(sock, msg, BUFFERSIZE, 0);
        if (numBytesRcvd < 0)
            DieSysError("Failed to receive message");
    } while(numBytesRcvd > 0);

    printf("%s\n", parseMsg(msg, 0));
    close(sock);
}

// Reads the content in the open file and sends the chunks 
// of data to the server
size_t sendFile(FILE *fp, char *header, int sock)
{
    size_t headerLen = strlen(header);
    size_t readLen;
    size_t maxRead = BUFFERSIZE - 8 - headerLen;
    char *filecontent = (char *) malloc(maxRead);
    readLen = fread(filecontent, sizeof(char), maxRead, fp);
    if (ferror(fp) != 0)
        DieSysError("Error reading file");

    char msg[BUFFERSIZE];
    memset(msg, 0, BUFFERSIZE);
    strcat(msg, header);
    strcat(msg, "\\end");
    strcat(msg, filecontent);
    strcat(msg, "\\end");
    printf("Sending message [%s]\n", msg);
    size_t strLen = strlen(msg);
    ssize_t numBytes = send(sock, msg, strLen, 0);
    return readLen;
}

// Reads the commands from the command line prompts
char* cmdParse(int sock)
{
    char *command, *input, *filename = NULL;
    size_t inputSize;
    for (;;)
    {
        if (getline(&input, &inputSize, stdin) < 0)
            exit(1);

        command = strtok(input, " \n");
        if (memcmp(command, "exit", sizeof("exit")) == 0)
        {
            sendExit(sock);
            exit(0);
        }
        else if (memcmp(command, "select", sizeof("select")) == 0)
        {
            // Get filename
            strtok(NULL, " \n");
            command = strtok(NULL, " \n");
            if (!checkFile(command)) filename = NULL;
            else {
                filename = (char*)malloc(strlen(command) + 1);
                memcpy(filename, command, strlen(command));
                filename[strlen(command) + 1] = '\0';
            }
        }
        else if (memcmp(command, "send", sizeof("send")) == 0)
        {
            if (filename != NULL){
                return filename;
            }
            else
                printf("no file selected!\n");
        }
    }
}