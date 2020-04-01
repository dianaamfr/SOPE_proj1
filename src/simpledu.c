#include "simpledu.h"
#include "utils.h"
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

int main(int argc, char* argv[], char* envp[]){
   flagMask flags;
   DIR *dirp;
   struct stat stat_buf;
   long totalSize = 0;

   int oldStdout;
   bool isSubDir = false;
   
   blockSIGUSR1();

   //if USR1 signal is pending we are reading a subdirectory
   if(pendingSIGUSR1() == OK){
      //read Pipe with flags
      if(read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1)
         error_sys("Error reading pipe\n");
      isSubDir = true;

      // save stdout descriptor 
      oldStdout = atoi(argv[1]);
   }
   //otherwise this is the parent (main directory)
   else{
      // the arguments(flags) should be checked
      if(checkArgs(argc,argv,&flags) != OK){
         fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
         exit(ERRORARGS);
      }

      if(validatePath(flags.path) != OK){
         fprintf(stderr,"Invalid path: %s\n",flags.path);
         exit(ERRORARGS);
      }

      //the stdout descriptor should be saved to be sent to the child processes
      oldStdout = dup(STDOUT_FILENO);

      //print flags for testing purposee
      printFlags(&flags,"Running"); 
   }

   if(getStatus(flags.L,&stat_buf,flags.path)){
      fprintf(stderr, "Stat error in %s\n", flags.path);
      exit(ERRORARGS);
   }

   //if the user asks for the size of a directory
   if (S_ISDIR(stat_buf.st_mode)) {

      totalSize += currentDirSize(flags.B,flags.b,&stat_buf);

      //try to open the directory
      if ((dirp = opendir(flags.path)) == NULL) 
         fprintf(stderr, "Could not open directory %s\n", flags.path);

      //add subdirectories size
      totalSize += searchSubdirs(dirp, &flags, oldStdout);

      //get back to the beginning of the directory
      rewinddir(dirp);

      //add size of regular files and symbolic links
      totalSize += searchFiles(dirp, &flags, oldStdout);

      closedir(dirp);

      //write subDir Size
      if(isSubDir)
         write(STDOUT_FILENO,&totalSize,sizeof(long int));
      
      //Calculare final size based on B flag
      if(flags.B)
         totalSize = sizeInBlocks(totalSize,flags.size);
      else if(!flags.B && !flags.b)
         totalSize = sizeInBlocks(totalSize,1024);
   }

   else if (S_ISREG(stat_buf.st_mode)){ //if the size of a regular file is asked, then it should be returned even if the user doesn't specify --all
      totalSize = regularFileSize(&flags,&stat_buf);
   }

   else if (S_ISLNK(stat_buf.st_mode)){ //if the size of a regular file is asked, then it should be returned even if the user doesn't specify --all
      totalSize = symbolicLinkSize(&flags,&stat_buf);
   }

   //print the size of the directory or regular file
   //for -B size with size > 1 we do the calculation as in -B 1 and compute dirInfo.size in the end by dividing the total by the size specified
   
   dprintf(oldStdout,"%-8ld  %-10s\n", totalSize, flags.path);

   exit(OK);
}
