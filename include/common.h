#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFERSIZE 500

void DieSysError(const char* msg)
{
    perror(msg);
    exit(1);
}

void DieUsrError(const char* msg, const char* detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

// Checks for end of header/message
int checkEnd(const char* msg, int pos)
{
    return (msg[pos] == '\\' && msg[pos+1] == 'e' && msg[pos+2] == 'n' && msg[pos+3] == 'd');
}

// Reads the message from the buffer gets the content after the header
char* parseMsg(const char* msg, int start)
{
    int pos = start;
    for(;;pos++)
    {
        if(checkEnd(msg, pos))
            break;
    }
    char *content = (char *)malloc(pos + 1 - start);
    strncpy(content, (char *)msg + start, (pos - start));
    strcat(content, "\0");
    printf("Content parsed from %d to %d msg [%s]\n", start, pos, content);
    return content;
}