#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "utils.h"

void sigHandler(int signo){
    if(signo == SIGUSR1){
        logRECV_SIGNAL(SIGUSR1);
        // printf("SIGUSR1   -- %d -- %d\n",getpid(),getppid()); 
        kill(getpid(),SIGSTOP);
        logSEND_SIGNAL(SIGSTOP,getpid());
    }
}

int main(int argc, char *argv[]){

    gettimeofday(&start, NULL);

    if(logCREATE(argc,argv) == 1){
        printf("SOMETING WOROND\n");
        exit(1);
    }

    struct sigaction action;
    action.sa_handler = sigHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1,&action,NULL) < 0){
        fprintf(stderr,"Unable to install SIGUSR1 handler\n");
        exit(1);
    }

    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGINT,&action,NULL) < 0){
        fprintf(stderr,"Unable to install SIGINT handler\n");
        exit(1);
    }

    int count = 0;
    while(1){
        printf("Child   -- %d -- %d ---- count = %d   ## ENVAR = %s\n",getpid(),getppid(),count,getenv("LOG_FILENAME")); 
        logRECV_PIPE("received pipe...");
        sleep(3);
        count++;
    }

    logEXIT(0);
    return 0;
}