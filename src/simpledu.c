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

pid_t gid = -1;

int main(int argc, char * argv[], char * envp[]){

   gettimeofday(&start, NULL);
   setenv("LOG_FILENAME","simpledu.log",1);

   // Initially block SIGUSR2
   blockSIGUSR2();

   // Set parent pid environment variable only if in the father of all processes
   if(pendingSIGUSR2() != OK)
      setParentPid();

   flagMask flags;
   DIR *dirp;
   struct stat stat_buf;
   long totalSize = 0;

   int stdout_fd;

   struct sigaction action;

   attachSIGHandler(action, SIGINT, SIG_IGN);

   attachSIGHandler(action, SIGUSR1, SIG_IGN);
   
   // We are currently in a subprocess
   if (isChildProcess() == OK){
      // Read the flags from pipe
      if (read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1)
         error_sys("Error reading pipe\n");
      
      start = flags.startTime;
      logCREATE(argc,argv);
      
      char msg[sizeof(flagMask)];
      flagsToString(&flags,msg);
      logRECV_PIPE(msg);

      // Save old stdout descriptor 
      stdout_fd = atoi(argv[1]);     
   }
   // Otherwise, we are in the parent of all processes / main directory
   else{
      clearLogfile();

      logCREATE(argc,argv);
      flags.startTime = start;

      attachSIGHandler(action, SIGINT, sigHandler);

      attachSIGHandler(action, SIGUSR1, sigUSR1Handler);

      // The args/flags must be checked
      if (checkArgs(argc,argv,&flags) != OK){
         fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
         logEXIT(ERRORARGS);
         exit(ERRORARGS);
      }

      if (validatePath(flags.path) != OK){
         fprintf(stderr,"Invalid path: %s\n",flags.path);
         logEXIT(ERRORARGS);
         exit(ERRORARGS);
      }

      // The old stdout descriptor should be saved to be sent to the child processes
      stdout_fd = dup(STDOUT_FILENO);

      // printFlags(&flags,"Running"); 
   }

   if (getStatus(flags.L,&stat_buf,flags.path)){
      fprintf(stderr, "Stat error in %s\n", flags.path);
      logEXIT(ERRORARGS);
      exit(ERRORARGS);
   }

   // If the user asks for the size of a directory
   if (S_ISDIR(stat_buf.st_mode)) {

      totalSize += currentDirSize(flags.b,&stat_buf);

      // Opening the directory
      if ((dirp = opendir(flags.path)) == NULL) 
         fprintf(stderr, "Could not open directory %s\n", flags.path);

      // Adding subdirectories size
      totalSize += searchSubdirs(dirp, &flags, stdout_fd);

      // Returning to the beggining of the current directory
      rewinddir(dirp);

      // Adding the size of Regular Files and Symbolic Links
      totalSize += searchFiles(dirp, &flags, stdout_fd);

      closedir(dirp);

      // Writing subdirectory size to the pipe previously assigned
      if (isChildProcess() == OK)
         write(STDOUT_FILENO,&totalSize,sizeof(long int));
      
      char msg[16];
      sprintf(msg, "totalSize = %ld", totalSize);
      logSEND_PIPE(msg);

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

   printDirInfo(&flags, totalSize, stdout_fd);

   logENTRY(totalSize,flags.path);

   waitForSubprocesses();

   logEXIT(OK);

   exit(OK);
}
