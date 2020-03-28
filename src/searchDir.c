#include "searchDir.h"
#include "aux.h"
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
   //TODO: separar em função auxiliar
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
   
      //se encontrar subdiretorios
      if (S_ISDIR(stat_buf.st_mode) && strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0){
         int fd1[2], fd2[2];
         pid_t pid; 

         //cria os pipes 
         if (pipe(fd1) < 0 || pipe(fd2) < 0){
            fprintf(stderr,"%s\n","Pipe error!\n");
            exit(ERROR); 
         }

         //criar processo filho e verifica erro do fork
         if ((pid = fork()) < 0){
            fprintf(stderr,"%s\n","Fork error!\n");
            exit(ERROR); 
         }
            
         if(pid > 0){ //PARENT
            close(fd1[READ]); // Pipe 1 (pai -> filho) o processo pai vai escrever as flags no Pipe 1 (logo não lê do Pipe 1)
            close(fd2[WRITE]); // Pipe 2 (filho -> pai) o pai lê o tamanho ocupado pelo filho (subdiretorio) e por isso não escreve no Pipe 2

            write(fd1[WRITE],&flags,sizeof(flagMask)); // Pipe 1 (pai -> filho) o processo pai escreve as flags no Pipe 1
            close(fd1[WRITE]);

            read(fd2[READ],&subDirSize,sizeof(long int)); // Pipe 2 (filho -> pai) o pai lê o tamanho ocupado pelo filho (subdiretorio)
            close(fd2[READ]);

            // acrecenta-se ao tamanho total do diretorio atual o tamanho do seu subdiretorio
            totalSize += subDirSize;
         }

         else{ //CHILD
            close(fd1[WRITE]); // Pipe 1 (pai -> filho) o processo filho vai ler as flags no Pipe 1 (logo não escreve no Pipe 1)
            close(fd2[READ]); // Pipe 2 (filho -> pai) o filho escreve o seu tamanho no Pipe 2 e por isso não lê desse pipe

            dup2(fd1[READ],STDIN_FILENO); // Quando o filho ler (read) do STDIN vai na verdade ler do Pipe 1 (onde tem as flags)

            dup2(fd2[WRITE],STDOUT_FILENO); // Quando o filho escrever (write) no STDOUT vai na verdade escrever no Pipe 2

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
   //TODO: separar em função auxiliar
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