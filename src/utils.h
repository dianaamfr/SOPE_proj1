#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void writetolog(char* logString)
{
    time_t rawtime;
    struct tm * timeinfo;
    char output[50];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(output, "[%d %d %d %d:%d:%d]",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    int logFd = 0;
    char logBuffer[200];
    sprintf(logBuffer,"%s -- %s -- %d\n",output,logString,getpid());
    /* ... */
    //Prepends date and time, and formats everything nicely into a single char[]
    if ((logFd = open(getenv("LOG_FILENAME"), O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0) {
        write(logFd, logBuffer, strlen(logBuffer));
        close(logFd);
    }
}