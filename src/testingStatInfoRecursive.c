#include <math.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* -> COMPILAR E EXECUTAR (exemplos):

   gcc -Wall testingStatInfoRecursive.c -lm         ->  Para compilar é necessário colocar -lm  devido ao uso da função ceil() 
   ./a.out . (mostra tamanho ocupado em disco do diretório atual)
   ./a.out ..
 

   ->DESCRIÇÃO

   Função recursiva para devolver o tamanho dos diretórios.

   Sem usar forks (faz tudo no mesmo processo) percorre recursivamente os diretórios de um diretório passado como argumento (argv[1]).

   Criei esta função com base nos exemplos da aula de diretórios e ficheiros e também de exemplos que encontrei na net.
   O meu objetivo era perceber onde podia ir buscar os valores que se obtém ao chamar o comando "du" na consola para um determinado diretório.

   Achei importante antes de começar a pensar na aplicação como multiprocesso perceber estes aspetos.

   Assim, procurei implementar algumas das funcionalidades do comando "du", particularmente:
   (NOTA: VER COMPILAÇÃO pois é igual em todos os casos, sendo necessário apenas comentar o código da 
   funcionalidade ativa e descomentar o código da que queremos ver (identificado com numeros))

   1) du -l <dir>             -> mostra o espaço ocupado em disco pelo diretório e pelos seus subdiretórios em nº de blocos de 1024 bytes
   2) du -l <dir> -B 1        -> "" em nº de blocos do sistema (4096 bytes)
   3) du -l <dir> -b          -> "" em bytes
   4) todas as funções anteriores com a opção -all (ou -a) que mostra também o espaço ocupado pelos ficheiros

   (Tentei ir buscar os valores tal como "du <dir>" devolvia na consola)
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
         totalSize += ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);          //1) du -l <dir>
         //totalSize += stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);                   //2) du -l <dir> -B 1
         //totalSize += stat_buf.st_size;                                                                         //3) du -l <dir> -b

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
         
      totalSize = ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);              //1) du -l <dir>
      //totalSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);                       //2) du -l <dir> -B 1
      //totalSize = stat_buf.st_size;                                                                             //3) du -l <dir> -b
   }
    
   //a usar se também queremos mostrar na consola o tamanho dos ficheiros em bytes
   //printf("%-10lld  %-10s\n", totalSize, name);                                                                 //4) du .... -all
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