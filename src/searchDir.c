#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include "simpledu.h"
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>

int main(int argc, char* argv[], char* envp[]){

   flagMask flags; //the flags will be received via pipe
   DIR *dirp; 
   struct dirent *direntp;
   struct stat stat_buf;
   long totalSize = 0, tempSize = 0;

   if(argc < 2){//for now it expects to receive only the path in its arguments
      printf("Usage: %s [path] \n",argv[0]);
      exit(ERRORARGS);
   }

   if(!flags.L){ //use l stat if -L was not specified - show info about the link itself
      if (lstat(flags.path, &stat_buf)){ 
         fprintf(stderr, "Stat error in %s\n", argv[1]);
         return ERRORARGS;
      }
   }
   else{ //use stat to follow symbolic links - dereference the link
      if (stat(flags.path, &stat_buf)){ 
         fprintf(stderr, "Stat error in %s\n", argv[1]);
         return ERRORARGS;
      }
   }

   //sum the size of the current directory according to active options
   if (flags.b){
      tempSize = stat_buf.st_size;
   }
   else if (flags.B && !flags.b) { //-B SIZE
      tempSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);
   }
   else{// du without options - default
      tempSize = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
   }
   totalSize += tempSize;

   //try to open the directory
   if ((dirp = opendir(argv[1])) == NULL) 
      fprintf(stderr, "Could not open directory %s\n", flags.path);

   //search for subdirectories in current directory
   while ((direntp = readdir(dirp)) != NULL) {

      char *pathname; //para guardar o path de cada ficheiro ou subdiretório

      pathname = malloc(strlen(flags.path) + 1 + strlen(direntp->d_name) + 1);

      if (pathname == NULL) {
         fprintf(stderr, "Memory Allocation error\n");
         exit(1);
      }
   
      //guarda o path do subdiretorio
      sprintf(pathname, "%s/%s", flags.path, direntp->d_name);

      if(!flags.L){ //use l stat if -L was not specified - show info about the link itself
         if (lstat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            return 1;
         }
      }
      else{
         if (stat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            return 1;
         }
      }
   
         if (S_ISDIR(stat_buf.st_mode)){
         //fork, exec and stuff;  Needs to get the size of the subdirectory from its child process and sum the size to the total size
         }

      free(pathname);
   }

   //get back to the beginning of the directory
   rewinddir(direntp);

   //search for regular files and symbolic links in current directory
   while ((direntp = readdir(dirp)) != NULL) {
      char *pathname; //para guardar o path de cada ficheiro ou subdiretório

      pathname = malloc(strlen(flags.path) + 1 + strlen(direntp->d_name) + 1);

      if (pathname == NULL) {
         fprintf(stderr, "Memory Allocation error\n");
         exit(1);
      }
   
      //guarda o path do ficheiro
      sprintf(pathname, "%s/%s", flags.path, direntp->d_name);

      if(!flags.L){ //use l stat if -L was not specified - show info about the link itself
         if (lstat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            return 1;
         }
      }
      else{
         if (stat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            return 1;
         }
      }
   
      if (S_ISREG(stat_buf.st_mode) || S_ISLNK(stat_buf.st_mode)){
         totalSize += dirFileSize(&flags,&stat_buf,pathname);
      } 

      free(pathname);  
   }

   closedir(dirp);

   //for -B size with size > 1 we do the calculation as in -B 1 and compute totalSize in the end by dividing the total by the size specified
   if(flags.B){
      totalSize  = ceil((double)totalSize / flags.size);
   }

   //print the size of the directory or regular file
   printf("%-8ld  %-10s\n", totalSize, flags.path);
}