#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include "simpledu.h"
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

long int checkBsize(char *optarg) {

   int alphabet = 0, number = 0;
   for (int i = 0; optarg[i] != '\0'; i++) {
      if (isalpha(optarg[i])) alphabet++;
      else if (isdigit(optarg[i])) number++;
   }

   if (alphabet != 0 && number != 0) return ERROR_BSIZE;
   else if (number > 0 && alphabet == 0 && strcmp(optarg, "0") != 0) return atoi(optarg);
   else if (alphabet == 1 && number == 0) {
      if (strcmp(optarg, "K") == 0) return 1024;
      else if (strcmp(optarg, "M") == 0) return pow(1024,2);
      else if (strcmp(optarg, "G") == 0) return pow(1024,3);
      else if (strcmp(optarg, "T") == 0) return pow(1024,4);
      else if (strcmp(optarg, "P") == 0) return pow(1024,5);
      else if (strcmp(optarg, "E") == 0) return pow(1024,6);
      else return ERROR_BSIZE;
   }
   else if (alphabet == 2 && number == 0) {
      if (strcmp(optarg, "KB") == 0) return 1000;
      else if (strcmp(optarg, "MB") == 0) return pow(1000,2);
      else if (strcmp(optarg, "GB") == 0) return pow(1000,3);
      else if (strcmp(optarg, "TB") == 0) return pow(1000,4);
      else if (strcmp(optarg, "PB") == 0) return pow(1000,5);
      else if (strcmp(optarg, "EB") == 0) return pow(1000,6);
      else return ERROR_BSIZE;
   }
   else return ERROR_BSIZE;

   return OK;
}

int validatePath(char * path){

   struct stat stat_buf;

   //if empty path consider the current directory
   if(strcmp(path, "") == OK){
      memset(path,0,MAX_PATH);
      strcpy(path, ".");
      return OK;
   }

  if(lstat(path, &stat_buf) == OK) return OK;
  return ERRORARGS;
}

int checkArgs(int argc, char* argv[], flagMask *flags){
   
   int c;
   int option_index = 0;
   long int size_b;

   static flagMask tempFlags;
   tempFlags.l = 1;

   /* DEBUGGING ...*/

   printf("###########################\n");

   printf("TESTING...\n");

   printf("a: %d\n", tempFlags.a);

   printf("b: %d\n", tempFlags.b);

   printf("B: %d\n", tempFlags.B);

   printf("l: %d\n", tempFlags.l);

   printf("L: %d\n", tempFlags.L);

   printf("S: %d\n", tempFlags.S);

   printf("max-depth: %d", tempFlags.d);
   if(tempFlags.d)
      printf(" value=%d", tempFlags.N);
   printf("\n");

   printf("size: %ld\n",tempFlags.size);
   
   printf("path: %s\n", tempFlags.path);

   printf("###########################\n");

   /*...*/
   
   while(1){         
      
      static struct option long_options[] = {
         {"all",           no_argument,         &tempFlags.a,     1 },
         {"bytes",         no_argument,         &tempFlags.b,     1 },
         {"block-size=",   required_argument,   0,              'B' },
         {"count-links",   no_argument,         &tempFlags.l,     1 },
         {"dereference",   no_argument,         &tempFlags.L,     1 },
         {"separate-dirs", no_argument,         &tempFlags.S,     1 },
         {"max-depth=",    required_argument,   0,              'd' },
         {0,               0,                   0,                0 }
      };

      c = getopt_long(argc, argv, "abB:lLSd:", long_options, &option_index);

      if (c == -1)
         break;
      
      switch (c){

         case 0:
            // printf("option %s", long_options[option_index].name);
            if (optarg)
               // printf(" with arg %s", optarg);
            // printf("\n");
            break;

         case 'a':
            // printf("option a\n");
            tempFlags.a = 1;
            break;

         case 'b':
            // printf("option b\n");
            tempFlags.b = 1;
            break;
         case 'B':
            // printf("option (B) block-size with value '%s'\n", optarg);
            tempFlags.B = 1;
            size_b = checkBsize(optarg);
            if (size_b == ERROR_BSIZE){
               fprintf(stderr, "%s: invalid -B argument: '%s'\n",argv[0],optarg);
               return ERRORARGS;
            }
            tempFlags.size = size_b;
            break;

         case 'l':
            // printf("option l\n");
            tempFlags.l = 1;
            break;

         case 'L':
            // printf("option L\n");
            tempFlags.L = 1;
            break;

         case 'S':
            // printf("option S\n");
            tempFlags.S = 1;
            break;
         
         case 'd':
            // printf("option (d) max-depth with value '%s'\n", optarg);
            tempFlags.d = 1;
            tempFlags.N = atoi(optarg);
            break;

         case '?':
            /* getopt_long already printed an error message. */
            // printf("Exiting...\n");
            return ERRORARGS;
            break;

         default:
            printf("Error: getopt returned character code 0%o \n", c);
            return ERRORARGS;
            break;
        }
   }

   if (optind < argc){
      // printf("non-option ARGV-elements: ");
      while (optind < argc){
         // printf("%s ", argv[optind]);
         sprintf(tempFlags.path, "%s", argv[optind++]);
         //sprintf(tempFlags.path + strlen(tempFlags.path), "%s ", argv[optind++]);  //in case we want multiple files
      }
      // printf("\n");
      // printf("PATH IS %s\n", tempFlags.path);
   }

   flags->a = tempFlags.a;
   flags->b = tempFlags.b;
   flags->B = tempFlags.B;
   flags->l = 1;
   flags->L = tempFlags.L;
   flags->S = tempFlags.S;
   flags->d = tempFlags.d;
   flags->N = tempFlags.N;
   flags->size = tempFlags.size;

   /*if(strcmp(tempFlags.size,"") == 0)
      strcpy(flags->size,"1");
   else
      strcpy(flags->size, tempFlags.size);*/
   
   
   strcpy(flags->path, tempFlags.path);
   
   return OK;
}

int main(int argc, char* argv[], char* envp[]){

   flagMask flags;
   DIR *dirp;
   struct dirent *direntp;
   struct stat stat_buf;
   long totalSize = 0, tempSize = 0, sizeBTemp = 0;

   if(checkArgs(argc,argv,&flags) != OK){
      printf("Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(ERRORARGS);
   }

  /*  if (flags.d < 0 || flags.size < 0) {
      exit(ERRORARGS);
   } */

   printf("###########################\n");

   printf("RUNNING...\n");

   printf("a: %d\n", flags.a);

   printf("b: %d\n", flags.b);

   printf("B: %d\n", flags.B);

   printf("l: %d\n", flags.l);

   printf("L: %d\n", flags.L);

   printf("S: %d\n", flags.S);

   printf("max-depth: %d", flags.d);
   if(flags.d)
      printf(" value=%d", flags.N);
   printf("\n");

   printf("size: %ld\n",flags.size);

   printf("path: %s\n", flags.path);

   printf("###########################\n");


   if(validatePath(flags.path) != OK){
      fprintf(stderr, "Invalid path error in %s\n", flags.path);
      exit(ERRORARGS);
   } 
      

   if (lstat(flags.path, &stat_buf)){ 
      fprintf(stderr, "Stat error in %s\n", flags.path);
      return 1;
   }

   //if the user asks for the size of a directory
   if (S_ISDIR(stat_buf.st_mode)) {

      //sum the size of the current directory according to active options
      if (flags.b){
         tempSize = stat_buf.st_size;
      }
      else if (flags.B) {
         tempSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);
      }
      else{
         tempSize = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
      }
      totalSize += tempSize;

      //try to open the directory
      if ((dirp = opendir(flags.path)) == NULL) 
         fprintf(stderr, "Could not open directory %s\n", flags.path);

      //while there are still contents inside the current directory
      while ((direntp = readdir(dirp)) != NULL) {

         char *pathname; //para guardar o path de cada ficheiro ou subdiretório

         pathname = malloc(strlen(flags.path) + 1 + strlen(direntp->d_name) + 1);

         if (pathname == NULL) {
            fprintf(stderr, "Memory Allocation error\n");
            exit(1);
         }
      
         //guarda o path do ficheiro ou subdiretorio
         sprintf(pathname, "%s/%s", flags.path, direntp->d_name);

         if (lstat(pathname, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", pathname);
            return 1;
         }

         if (S_ISREG(stat_buf.st_mode)) {

            /*if is dir sum here the size of dir - check for hidden folder too; and do the fork()*/

            //calculate the space in disk of the regular file according to the active options
            if (flags.b){
               tempSize = stat_buf.st_size;
            }
            else if (flags.B) {
               tempSize  = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);
               sizeBTemp = tempSize;
               tempSize  = ceil((double)tempSize / flags.size);
            }
            else{
               tempSize = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
            }

            //print all regular files if --all (-a) is active
            if(flags.a) printf("%-8ld  %-10s\n", tempSize, pathname);
            if(!flags.B) totalSize += tempSize;
            else totalSize += sizeBTemp;
         }
         free(pathname);
      }
      closedir(dirp);
   }
   else if (S_ISREG(stat_buf.st_mode)){ //if the size of a regular file is asked, then it should be returned even if the user doesn't specify --all

      if (flags.b){
         totalSize = stat_buf.st_size;
      }
      else if (flags.B) {
         totalSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);
         totalSize = ceil((double)totalSize / flags.size);
      }
      else{
         totalSize = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
      }
   }

   if(flags.B) totalSize  = ceil((double)totalSize / flags.size);
   //print the size of the directory or regular file
   printf("%-8ld  %-10s\n", totalSize, flags.path);

   exit(OK);
}

