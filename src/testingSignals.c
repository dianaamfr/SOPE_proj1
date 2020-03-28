#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "utils.h"

void sigHandler(int signo){
    if(signo == SIGINT){
        printf("STOPPING!! -- %d -- %d\n",getpid(),getppid());
        
        kill(-getpgrp(),SIGUSR1);
        
        char c;
        printf( "Continue? (Y or N) ");
        scanf("%c",&c);
        while ((getchar()) != '\n');

        printf( "You entered: %c\n", c);
        
        if(c == 'Y'){
            printf("CONTINUING!! -- %d -- %d\n", getpid(),getppid());
            kill(-getpgrp(),SIGCONT);
        }
        else{
            printf("TERMINATING!! -- %d -- %d\n", getpid(),getppid());
            kill(-getpgrp(),SIGTERM);
        }
    }
}

int main(void){
    int pid, ppid = getppid();
    setenv("LOG_FILENAME","simpledu.log",1);

    printf("I AM THE PARENT -- %d -- %d\n",getpid(),getppid());

    for(int i = 0; i < 5; i++)
        if(getppid() == ppid)
            pid = fork();

    if(pid == 0){
        char *a[] = {"./tlogs.o",NULL};
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

            while(1){
                printf("Parent  -- %d -- %d\n",getpid(),getppid());
                writetolog("I am your father!");
                sleep(1);
            }
        }

    }
    exit(0);
}