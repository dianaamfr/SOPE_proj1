#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "logging.h"

int appendLogInfo(action a, char * info){

    int logFd = 0;

    if ((logFd = open(getenv("LOG_FILENAME"), O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0) {

        char *actions[] = {"CREATE", "EXIT", "RECV_SIGNAL", "SEND_SIGNAL", "RECV_PIPE", "SEND_PIPE", "ENTRY"};

        struct timeval stop;
        gettimeofday(&stop, NULL);

        double time = (double) (stop.tv_sec - start.tv_sec) * 1000.0 + (double) (stop.tv_usec - start.tv_usec) / 1000.0;
        char logBuffer[200];

        sprintf(logBuffer,"%10.2f - %08d - %11s - %s\n",time,getpid(),actions[a],info);
        
        write(logFd, logBuffer, strlen(logBuffer));
        close(logFd);

        return 0;
    }

    return 1;
}

int logCREATE(int argc, char* args[]){

    char info[200] = "";
    
    if(argc > 0)
        for(int i = 0; i < argc; i++){
            strcat(info,args[i]);
            if(i < argc - 1){
                strcat(info,", ");
            }
        }

    else
        sprintf(info,"%s","void");

    return appendLogInfo(CREATE,info);
}

int logEXIT(int termCode){

    char info[10] = "";
    
    sprintf(info,"%d",termCode);

    return appendLogInfo(EXIT,info);
}

int logRECV_SIGNAL(int SIG){

    char info[10] = "";
    
    sprintf(info,"%d",SIG);
    
    return appendLogInfo(RECV_SIGNAL,info);
}

int logSEND_SIGNAL(int SIG, int pid){

    char info[20] = "";
    
    sprintf(info,"%d sent to %08d", SIG, pid);

    return appendLogInfo(SEND_SIGNAL,info);
}

int logRECV_PIPE(char * msg){

    char info[200] = "";
    
    sprintf(info,"\' %s \'",msg);       

    return appendLogInfo(RECV_PIPE,msg);
}

int logSEND_PIPE(char * msg){

    char info[200] = "";
    
    sprintf(info,"\' %s \'",msg);  

    return appendLogInfo(SEND_PIPE, msg);
}

int logENTRY(int bytes, char * path){

    char info[200] = "";
    
    sprintf(info,"%d at %s", bytes, path);

    return appendLogInfo(ENTRY, info);
}
