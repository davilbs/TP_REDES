#include <stdio.h>
#include <string.h>

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

int checkEnd(const char* msg, int pos)
{
    return (msg[pos] == '\\' && msg[pos+1] == 'e' && msg[pos+2] == 'n' && msg[pos+3] == 'd');
}

// Reads the message from the buffer and splits at the \end
char* parseMsg(const char* msg)
{
    char *content;
    int pos = 0;
    for(;;pos++)
    {
        if(checkEnd(msg, pos))
            break;
    }
    content = malloc(sizeof(char)*(pos + 1));
    strncat(content, msg, pos);
    strcat(content, "\0");
    return content;
}