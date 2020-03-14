#include <math.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Função recursiva para devolver o tamanho dos diretórios

   Sem usar forks (faz tudo no mesmo processo) percorre recursivamente os diretórios de um diretório passado como argumento.

   Criei esta função com base nos exemplos da aula de diretórios e ficheiros e também de exemplos que encontrei na net.
   O meu objetivo era perceber onde podia ir buscar os valores que se obtém ao chamar o comando "du" na consola para um determinado diretório.

   Achei importante antes de começar a pensar na aplicação como multiprocesso perceber estes aspetos.

   Assim, procurei que as funções permitessem implementar algumas das funcionalidades do comando "du", particularmente:
   1) du -l <dir>
   2) du -l <dir> -B 1
   3) du -l <dir> -b 

   (Tentei ir buscar os valores tal como "du <dir>" devolvia na consola)
   Estão comentadas com o respetivo número identificador as partes do código que permitem executar cada uma das funcionalidades.
*/

long long dirSize(const char *name) {
   DIR *dirp;
   struct dirent *direntp;
   struct stat stat_buf;
   long long totalSize = 0; //para guardar o tamanho total em disco ocupado pelo diretório passado nos argumentos

   if (lstat(name, &stat_buf)) { //don't follow symbolic links (para a opção -L provavelmente temos que mudar algo aqui)
      fprintf(stderr, "Stat error in %s\n", name);
      return 1;
   }
   if (S_ISDIR(stat_buf.st_mode)) { //no caso de o ficheiro ser um diretório

   if ((dirp = opendir(name)) == NULL)
         fprintf(stderr, "Could not open directory %s\n", name);
   else {
      //Soma o tamanho do próprio diretório
      totalSize += ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);  //1) du -l <dir>
      //totalSize += stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);  //2) du -l <dir> -B 1
      //totalSize += stat_buf.st_size; //3) du -l <dir> -b

      //enquanto o diretório tem conteúdos para ler
      while ((direntp = readdir(dirp)) != NULL) {
            char *pathname; //para guardar o path de cada ficheiro ou subdiretório

            // ignore diretório corrente(".") e pai("..")
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
               continue;

            pathname = malloc(strlen(name) + 1 + strlen(direntp->d_name) + 1);
            if (pathname == NULL) {
               fprintf(stderr, "Memory Allocation error\n");
               exit(1);
            }
            
            //guarda o path do ficheiro ou subdiretorio
            sprintf(pathname, "%s/%s", name, direntp->d_name);
            //soma o tamanho do subdiretorio ou ficheiro ao tamanho total fazendo uma chamada recursiva
            totalSize += dirSize(pathname);
         
            free(pathname);
      }
      closedir(dirp);
   }
   //a usar se só queremos mostrar na consola o tamanho dos diretórios
   printf("%-10lld  %-10s\n", totalSize, name);
      
   }else{
      //se for um ficheiro regular soma o seu tamanho ao tamanho total do diretório
         
      totalSize = ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024); //1) du -l <dir>
      //totalSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize); //2) du -l <dir> -B 1
      //totalSize = stat_buf.st_size; //3) du -l <dir> -b
   }
    
   //a usar se também queremos mostrar na consola o tamanho dos ficheiros em bytes
   //printf("%-10lld  %-10s\n", totalSize, name);
   return totalSize;
}

int main(int argc, char *argv[]) {
   if (argc != 2){
      fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
      return 1;
   }

   dirSize(argv[1]); 
   
   return 0;
}