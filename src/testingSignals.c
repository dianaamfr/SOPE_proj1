#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

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

    if(signo == SIGUSR1){
        // printf("SIGUSR1   -- %d -- %d\n",getpid(),getppid()); 
        kill(getpid(),SIGSTOP);
    }
}

int main(void){
    int pid, ppid = getppid();

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

    printf("I AM THE PARENT -- %d -- %d\n",getpid(),getppid());

    pid = fork();

    if(pid == 0){
        int count = 0;
        while(1){
            printf("Child   -- %d -- %d ---- count= %d\n",getpid(),getppid(),count); 
            sleep(1);
            count++;
        }
        printf("CHILD SIGINT   -- %d -- %d\n",getpid(),getppid()); 
    }
    else if(pid > 0){
        if(getppid() == ppid){           
            
            printf("Parent  -- %d -- %d\n",getpid(),getppid());
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
                sleep(1);
            }
        }

    }
    exit(0);
}