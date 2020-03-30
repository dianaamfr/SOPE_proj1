#include "simpledu.h"
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
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

int main(int argc, char* argv[], char* envp[]){
   flagMask flags;
   DIR *dirp;
   struct dirent *direntp;
   struct stat stat_buf;
   long totalSize = 0;
   long int subDirSize;
   int oldStdout;
   char oldStdoutStr[10] = "";
   bool isSubDir = false;
   sigset_t pending_signals;
   sigset_t mask;

   // temporarily block SIGUSR1
   sigemptyset (&mask);
   sigaddset (&mask, SIGUSR1);
   //set new mask
   sigprocmask(SIG_BLOCK, &mask, NULL);

   if(checkArgs(argc,argv,&flags) != OK){
      fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(ERRORARGS);
   }
   
   //se o path não for válido então:
   // 1) O argumento é o descritor do stdout -> processo vai ler subdiretorio e precisa de ler as flags do pipe
   // 2) O path é inválido
   if(validatePath(flags.path) != OK){
      // 1) Se há algum sigusr1 pendente então deve ler-se as flags do pipe
       if(sigpending (&pending_signals) == 0 && sigismember (&pending_signals, SIGUSR1)){
         if(read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1){ 
            fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
            exit(ERRORARGS);
         }
      } 
      // 2) Não há sigusr1 pendentes logo o path é inválid
      else{
         fprintf(stderr,"Invalid path: %s\n",flags.path);
         exit(ERRORARGS);
      }
      
      if(read(STDIN_FILENO,&flags,sizeof(flagMask)) == -1){ 
         fprintf(stderr,"Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
         exit(ERRORARGS);
      }

      isSubDir = true;

      //se for um subdiretorio é necessário utilizar o descritor do stdout passado nos argumentos
      oldStdout = atoi(argv[1]);
      // Converter o descritor antigo do STDOUT em string para passar como parâmetro no exec
      sprintf(oldStdoutStr , "%d", oldStdout); 
   } 

   else{
      oldStdout = dup(STDOUT_FILENO);
      sprintf(oldStdoutStr , "%d", oldStdout);
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

      //search for subdirectories in current directory
      while ((direntp = readdir(dirp)) != NULL) {

         char * pathname = malloc(strlen(flags.path) + 1 + strlen(direntp->d_name) + 1);
         if (pathname == NULL) error_sys("Memory Allocation error\n");
         sprintf(pathname, "%s/%s", flags.path, direntp->d_name); //guarda o path do subdiretorio 

         if(getStatus(flags.L,&stat_buf,pathname)){
            fprintf(stderr, "Stat error in %s\n", pathname);
            exit(ERRORARGS);
         }

         //se encontrar subdiretorios
         if (S_ISDIR(stat_buf.st_mode) && strcmp(direntp->d_name, ".") != 0 && strcmp(direntp->d_name, "..") != 0){
            int fd1[2], fd2[2];
            pid_t pid; 

            //cria os pipes 
            if (pipe(fd1) < 0 || pipe(fd2) < 0)
               error_sys("Pipe error!\n");


            //criar processo filho e verifica erro do fork
            if ((pid = fork()) < 0) 
               error_sys("Fork error!\n");
               
            if(pid > 0){ //PARENT
               close(fd1[READ]); // Pipe 1 (pai -> filho) o processo pai vai escrever as flags no Pipe 1 (logo não lê do Pipe 1)
               close(fd2[WRITE]); // Pipe 2 (filho -> pai) o pai lê o tamanho ocupado pelo filho (subdiretorio) e por isso não escreve no Pipe 2

               char tempPath[MAX_PATH];
               strcpy(tempPath,flags.path);
               memset(flags.path,'\0',MAX_PATH);
               strcpy(flags.path, pathname);

               write(fd1[WRITE],&flags,sizeof(flagMask)); // Pipe 1 (pai -> filho) o processo pai escreve as flags no Pipe 1
               close(fd1[WRITE]);

               memset(flags.path,'\0',MAX_PATH);
               strcpy(flags.path,tempPath);

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
               kill(pid,SIGUSR1);
               execl("simpledu", "simpledu", oldStdoutStr, NULL);
               error_sys("Exec error!\n");
            } 
         }

         free(pathname);
      }

      //get back to the beginning of the directory
      rewinddir(dirp);

      //search for regular files and symbolic links in current directory
      totalSize += searchFiles(direntp, dirp, &stat_buf, &flags, oldStdout);

      closedir(dirp);

      if(isSubDir){
         write(STDOUT_FILENO,&totalSize,sizeof(long int));
      }

      if(flags.B)
         totalSize = sizeInBlocks(totalSize,flags.size);
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
