#include <stdio.h>

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