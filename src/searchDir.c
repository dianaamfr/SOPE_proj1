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
#include "searchDir.h"

long int dirFileSize(flagMask *flags, struct stat *stat_buf, char * pathname, int stdout_fd){
   
   long int sizeBTemp = 0, size = 0;

   if (S_ISREG(stat_buf->st_mode)){//if it is a regular file
      //calculate the space in disk of the regular file according to the active options
      // -b || -B 1 -b || -b -B 1  => -b
      if (flags->b && !flags->B){
         size = stat_buf->st_size;
      }
      else if(flags->B && !flags->b){
         size  = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
         sizeBTemp = size;
         size  = ceil((double)size / flags->size);
      }
      else if(flags->B && flags->b){
         size  = stat_buf->st_size;
         sizeBTemp = size;
         size  = ceil((double)size / flags->size);
      }
      else{ // du without options - default
         size = (int)ceil(stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize)/1024);
      }
   }

   else if(S_ISLNK(stat_buf->st_mode)){
      if(flags->b && !flags->B){// -b || -B 1 -b || -b -B 1  => -b
         size = stat_buf->st_size; //count size of the link itself in bytes
      }
      else{ 
         if(!flags->L){
            if(flags->B && flags->b){
               size  = stat_buf->st_size;
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else size = 0;
         }
         else{ //dereference symbolic links
            if (flags->B && !flags->b){
               size  = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else if(flags->B && flags->b){
               size  = stat_buf->st_size;
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else{//du without options - default
               size = (int)ceil(stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize)/1024);
            }
         }
         
      }
   }

   //print all regular files if --all (-a) is active
   if(flags->a)
      dprintf(stdout_fd,"%-8ld  %-10s\n", size, pathname);
   
   //for -B option we want to show one size on screen but pass another to the total size calculation
   if(flags->B) 
      size = sizeBTemp;

   return size;
}

int validatePath(char * path){

   struct stat stat_buf;

   //if empty path consider the current directory
   if(strcmp(path, "") == OK){
      memset(path,0,MAX_PATH);
      strcpy(path, ".");
      return OK;
   }

   if(lstat(path, &stat_buf) == OK) 
      return OK;
   
   return ERROR;
}

int main(int argc, char* argv[], char* envp[]){

   flagMask flags; //the flags will be received via pipe
   DIR *dirp; 
   struct dirent *direntp;
   struct stat stat_buf;
   long int tempSize = 0; 
   long int totalSize = 0, subDirSize;
   int stdout_fd;

   if(argc < 3){//for now it expects to receive only the path in its arguments
      printf("Usage: %s [path] \n",argv[0]);
      exit(ERROR);
   }

   if(read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1){ // lê do pipe as flags
      fprintf(stderr, "Flags test: %d %d %d %d!\n", flags.b, flags.B, flags.a, flags.l);
      exit(ERROR);
   }

   stdout_fd = atoi(argv[2]);

   //check if the path exists
   if(validatePath(argv[1]) != OK){
      fprintf(stderr, "Invalid path error in %s\n", argv[1]);
      exit(ERROR);
   }

   if(!flags.L){ //use l stat if -L was not specified - show info about the link itself
      if (lstat(argv[1], &stat_buf)){ 
         fprintf(stderr, "Stat error in %s\n", argv[1]);
         return ERROR;
      }
   }
   else{ //use stat to follow symbolic links - dereference the link
      if (stat(argv[1], &stat_buf)){ 
         fprintf(stderr, "Stat error in %s\n", argv[1]);
         return ERROR;
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
      fprintf(stderr, "Could not open directory %s\n", argv[1]);

   //search for subdirectories in current directory
   while ((direntp = readdir(dirp)) != NULL) {

      char *pathname; //para guardar o path de cada ficheiro ou subdiretório

      pathname = malloc(strlen(argv[1]) + 1 + strlen(direntp->d_name) + 1);

      if (pathname == NULL) {
         fprintf(stderr, "Memory Allocation error\n");
         exit(1);
      }
   
      //guarda o path do subdiretorio
      sprintf(pathname, "%s/%s", argv[1], direntp->d_name);

      if(!flags.L){ //use l stat if -L was not specified - show info about the link itself
         if (lstat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            exit(ERROR);
         }
      }
      else{
         if (stat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            exit(ERROR);
         }
      }
   
      if (S_ISDIR(stat_buf.st_mode) && strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0){
         int fd1[2], fd2[2];
         pid_t pid; 

         if (pipe(fd1) < 0 || pipe(fd2) < 0){
            fprintf(stderr,"%s\n","Pipe error!\n");
            exit(ERROR); 
         }

         if ((pid = fork()) < 0){
            fprintf(stderr,"%s\n","Fork error!\n");
            exit(ERROR); 
         }
            
         if(pid > 0){ //PARENT
            close(fd1[READ]);
            close(fd2[WRITE]);

            write(fd1[WRITE],&flags,sizeof(flagMask));
            close(fd1[WRITE]);

            read(fd2[READ],&subDirSize,sizeof(long int));
            close(fd2[READ]);

            totalSize += subDirSize;
         }

         else{ //CHILD
            close(fd1[WRITE]);
            close(fd2[READ]);

            dup2(fd1[READ],STDIN_FILENO);

            dup2(fd2[WRITE],STDOUT_FILENO);

            execl("searchDir", "searchDir", pathname, argv[2], NULL);
            fprintf(stderr,"Exec error in %s!\n",pathname);
            exit(ERROR);
         }   
      }

      free(pathname);
   }

   //get back to the beginning of the directory
   rewinddir(dirp);

   //search for regular files and symbolic links in current directory
   while ((direntp = readdir(dirp)) != NULL) {
      char *pathname; //para guardar o path de cada ficheiro ou subdiretório

      pathname = malloc(strlen(argv[1]) + 1 + strlen(direntp->d_name) + 1);

      if (pathname == NULL) {
         fprintf(stderr, "Memory Allocation error\n");
         exit(1);
      }
   
      //guarda o path do ficheiro
      sprintf(pathname, "%s/%s", argv[1], direntp->d_name);

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
         totalSize += dirFileSize(&flags,&stat_buf,pathname,stdout_fd);
      } 

      free(pathname);  
   }

   closedir(dirp);

   write(STDOUT_FILENO,&totalSize,sizeof(long int));

   //print the size of the directory or regular file
   //for -B size with size > 1 we do the calculation as in -B 1 and compute dirInfo.size in the end by dividing the total by the size specified
   if(flags.B)
      totalSize = ceil((double)totalSize / flags.size);
   dprintf(stdout_fd,"%-8ld  %-10s\n", totalSize, argv[1]);

   exit(OK);
}