#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include "logging.h"
#define READ 0
#define WRITE 1

void sigHandler(int signo){
    if(signo == SIGINT){
        logRECV_SIGNAL(SIGINT);
        printf("STOPPING!! -- %d -- %d\n",getpid(),getppid());
        
        logSEND_SIGNAL(SIGUSR1,-getpgrp());
        kill(-getpgrp(),SIGUSR1);
        
        char c;
        printf( "Continue? (Y or N) ");
        scanf("%c",&c);
        while ((getchar()) != '\n');

        printf( "You entered: %c\n", c);
        
        if(c == 'Y'){
            printf("CONTINUING!! -- %d -- %d\n", getpid(),getppid());
            logSEND_SIGNAL(SIGCONT,-getpgrp());
            kill(-getpgrp(),SIGCONT);
        }
        else{
            printf("TERMINATING!! -- %d -- %d\n", getpid(),getppid());
            logSEND_SIGNAL(SIGTERM,-getpgrp());
            logEXIT(0);
            kill(-getpgrp(),SIGTERM);
        }
    }
}

int main(void){

    gettimeofday(&start, NULL);
    printf("WHAT SHIT IS IN START PARENT: %ld -- %ld\n",start.tv_sec, start.tv_usec);

    int pipeStart[2];

    int pid, ppid = getppid();
    setenv("LOG_FILENAME","simpledu.log",1);

    if(logCREATE(0,NULL) == 1){
        printf("SOMETING WOROND\n");
        exit(1);
    }

    printf("I AM THE PARENT -- %d -- %d\n",getpid(),getppid());

    for(int i = 0; i < 5; i++)
        if(getppid() == ppid)
            pid = fork();

    pipe(pipeStart);

    if(pid == 0){
        dup2(pipeStart[READ], STDIN_FILENO);
        close(pipeStart[WRITE]);

        char *a[] = {"./tlogs.o","bla","ble","bli",NULL};
        execvp(a[0],a);
    }
    else if(pid > 0){
        if(getppid() == ppid){           
            
            printf("Parent  -- %d -- %d\n",getpid(),getppid());

            struct sigaction action;
            action.sa_handler = sigHandler;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            
            if (sigaction(SIGINT,&action,NULL) < 0){
                fprintf(stderr,"Unable to install SIGINT handler\n");
                exit(1);
            }

            action.sa_handler = SIG_IGN;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            
            if (sigaction(SIGUSR1,&action,NULL) < 0){
                fprintf(stderr,"Unable to install SIGINT handler\n");
                exit(1);
            }
            
            int fout = dup(STDOUT_FILENO);
            dup2(pipeStart[WRITE], STDOUT_FILENO);
            close(pipeStart[READ]);
            int error = 0;
            if(write(pipeStart[WRITE],&start.tv_sec,sizeof(time_t)) == -1)
                error = 1;
            dup2(fout,STDOUT_FILENO);
            printf("WHAT SHIT IS IN START PARENT: %ld -- %ld\n",start.tv_sec, start.tv_usec);
            if(error) exit(1);

            while(1){
                printf("Parent  -- %d -- %d\n",getpid(),getppid());
                if(rand() % 2 == 0)
                    logSEND_PIPE("pipe was sent...");
                else
                    logENTRY(rand()%64,"entry ...");
                sleep(1);
            }
        }

    }

    logEXIT(0);
    exit(0);
}