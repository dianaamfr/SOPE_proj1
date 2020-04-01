#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "simpledu.h"
#include "utils.h"
#include "logging.h"

int main(int argc, char * argv[], char * envp[]){
   
   gettimeofday(&start, NULL);
   setenv("LOG_FILENAME","simpledu.log",1);

   flagMask flags;
   DIR *dirp;
   struct stat stat_buf;
   long totalSize = 0;

   int oldStdout;
   bool isSubDir = false;
   
   blockSIGUSR1();

   struct sigaction action;
   action.sa_handler = sigHandler;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;

   if (sigaction(SIGUSR2,&action,NULL) < 0){
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

   if (pendingSIGUSR1() == OK){ // If SIGUSR1 is pending, then we are currently in a subdirectory
      
      // Read the flags from pipe
      if (read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1)
         error_sys("Error reading pipe\n");
      
      isSubDir = true;

      start = flags.startTime;
      logCREATE(argc,argv);

      // Save old stdout descriptor 
      oldStdout = atoi(argv[1]);
   }
   else{ // Otherwise, we are in the parent/main directory

      logCREATE(argc,argv);
      flags.startTime = start;

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
      
      if (sigaction(SIGUSR2,&action,NULL) < 0){
            fprintf(stderr,"Unable to install SIGINT handler\n");
            exit(1);
      }

      // The args/flags must be checked
      if (checkArgs(argc,argv,&flags) != OK){
         fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
         exit(ERRORARGS);
      }

      if (validatePath(flags.path) != OK){
         fprintf(stderr,"Invalid path: %s\n",flags.path);
         exit(ERRORARGS);
      }

      // The old stdout descriptor should be saved to be sent to the child processes
      oldStdout = dup(STDOUT_FILENO);

      printFlags(&flags,"Running"); 
   }

   if (getStatus(flags.L,&stat_buf,flags.path)){
      fprintf(stderr, "Stat error in %s\n", flags.path);
      exit(ERRORARGS);
   }

   // If the user asks for the size of a directory
   if (S_ISDIR(stat_buf.st_mode)) {

      totalSize += currentDirSize(flags.B,flags.b,&stat_buf);

      // Opening the directory
      if ((dirp = opendir(flags.path)) == NULL) 
         fprintf(stderr, "Could not open directory %s\n", flags.path);

      // Adding subdirectories size
      totalSize += searchSubdirs(dirp, &flags, oldStdout);

      // Returning to the beggining of the current directory
      rewinddir(dirp);

      // Adding the size of Regular Files and Symbolic Links
      totalSize += searchFiles(dirp, &flags, oldStdout);

      closedir(dirp);

      // Writing subdirectory size to the pipe previously assigned
      if (isSubDir)
         write(STDOUT_FILENO,&totalSize,sizeof(long int));
      
      // Calculating the final size based on -B flag
      if (flags.B)
         totalSize = sizeInBlocks(totalSize,flags.size);
      else if (!flags.B && !flags.b)
         totalSize = sizeInBlocks(totalSize,1024);
   }

   else if (S_ISREG(stat_buf.st_mode)){ 
      // If the size of a regular file is asked
      // Then it should be returned even 
      // if the user doesn't specify --all

      totalSize = regularFileSize(&flags,&stat_buf);
   }

   else if (S_ISLNK(stat_buf.st_mode)){ 
      // If the size of a symbolic link is asked, 
      // Then it should be returned even 
      // if the user doesn't specify --all

      totalSize = symbolicLinkSize(&flags,&stat_buf);
   }

   // Printing the size of the directory or regular file
   // For -B with size_b > 1, the calculation is done as -B size_b=1
   // and computed dirInfo.size in the end by dividing the total size by the size_b specified
   
   dprintf(oldStdout,"%-8ld  %-10s\n", totalSize, flags.path);

   exit(OK);
}
