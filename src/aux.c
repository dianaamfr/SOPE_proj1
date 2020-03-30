#include "aux.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

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

void printFlags(flagMask * flags, char * description){
   printf("###########################\n");

   printf("%s...\n",description);

   printf("a: %d\n", flags->a);

   printf("b: %d\n", flags->b);

   printf("B: %d\n", flags->B);

   printf("l: %d\n", flags->l);

   printf("L: %d\n", flags->L);

   printf("S: %d\n", flags->S);

   printf("max-depth: %d", flags->d);
   if(flags->d)
      printf(" value=%d", flags->N);
   printf("\n");

   printf("size: %ld\n",flags->size);

   printf("path: %s\n", flags->path);

   printf("###########################\n");
}

long int checkBsize(char *optarg) {

   int alphabet = 0, number = 0;
   for (int i = 0; optarg[i] != '\0'; i++) {
      if (isalpha(optarg[i])) 
         alphabet++;
      else if (isdigit(optarg[i])) 
         number++;
   }

   if (alphabet != 0 && number != 0) 
      return ERROR_BSIZE;
   
   else if (number > 0 && alphabet == 0 && strcmp(optarg, "0") != 0) 
      return atoi(optarg);

   else if (alphabet == 1 && number == 0) {
      if (strcmp(optarg, "K") == 0) return 1024;
      else if (strcmp(optarg, "M") == 0) return pow(1024,2);
      else if (strcmp(optarg, "G") == 0) return pow(1024,3);
      else if (strcmp(optarg, "T") == 0) return pow(1024,4);
      else if (strcmp(optarg, "P") == 0) return pow(1024,5);
      else if (strcmp(optarg, "E") == 0) return pow(1024,6);
      else 
         return ERROR_BSIZE;
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

int checkArgs(int argc, char* argv[], flagMask *flags){
   
   int c;
   int option_index = 0;
   long int size_b;

   static flagMask tempFlags;
   tempFlags.l = 1;

   /* DEBUGGING ...*/
   printFlags(&tempFlags, "TESTING");
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

            if(tempFlags.B){ //if -B SIZE ... -b assume -b (the last option is assumed) => disable -B 
               tempFlags.size = 0;
               tempFlags.B = 0;
            }

            break;
         case 'B':
            // printf("option (B) block-size with value '%s'\n", optarg);
            tempFlags.B = 1;
            size_b = checkBsize(optarg);
            if (size_b == ERROR_BSIZE){
               fprintf(stderr, "%s: invalid -B argument: '%s'\n",argv[0],optarg);
               return ERRORARGS;
            }

            if(size_b == 1 && tempFlags.b){ //if -B 1 and -b assume -b (order is irrelevant)
               tempFlags.size = 0;
               tempFlags.B = 0;
            }
            else tempFlags.size = size_b; //if -b is not active 
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

double sizeInBlocks(long int totalSize, long int Bsize){
   return ceil((double)totalSize / Bsize);
}

long int regularFileSize(flagMask *flags, struct stat *stat_buf){
   
   long int totalSize=0;

   // -b || -B 1 -b || -b -B 1  => -b
   if (flags->b && !flags->B){
      return stat_buf->st_size;
   }
   else if (flags->B && !flags->b){
      totalSize = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
      return sizeInBlocks(totalSize,flags->size);
   }
   else if(flags->B && flags->b){ // -b -B SIZE with SIZE > 1
      return sizeInBlocks(stat_buf->st_size,flags->size);
   }
   else{//du without options - default
      totalSize = stat_buf->st_blksize * ceil((double)stat_buf->st_size/stat_buf->st_blksize);
      return sizeInBlocks(totalSize,1024);
   }
}

long int symbolicLinkSize(flagMask *flags, struct stat *stat_buf){

   long int totalSize=0;
   
   if (flags->b && !flags->B){ // -b || -B 1 -b || -b -B 1  => -b
      return stat_buf->st_size; //count size of the link itself in bytes
   }
   else{ 
      if(!flags->L){
            if(flags->B && flags->b){
               return sizeInBlocks(stat_buf->st_size,flags->size);
            }
            return 0;
      }
      else{ //dereference symbolic links
         if (flags->B && !flags->b){
            totalSize = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
         return sizeInBlocks(totalSize,flags->size);
         }
         else if(flags->B && flags->b){
            return sizeInBlocks(stat_buf->st_size,flags->size);
         }
         else{//du without options - default
            totalSize = stat_buf->st_blksize * ceil((double)stat_buf->st_size/stat_buf->st_blksize);
            return sizeInBlocks(totalSize,1024);
         }
      }
      
   }
}

void error_sys(char *msg){
   fprintf(stderr,"%s\n",msg);
   exit(ERROR);
} 

int getStatus(int flag_L, struct stat * stat_buf, char * path){
   //use l stat if -L was not specified - show info about the link itself
   if(!flag_L){
      if (lstat(path, stat_buf)) 
         return ERRORARGS;
   }
   //use stat to follow symbolic links - dereference the link
   else{
      if (stat(path, stat_buf))
         return ERRORARGS;
   }
   return OK;
}

int currentDirSize(int flags_B, int flags_b, struct stat * stat_buf){
   //sum the size of the current directory according to active options
   if (flags_b){
      return stat_buf->st_size;
   }
   else if (flags_B && !flags_b) { //-B SIZE
      return stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
   }
   else{// du without options - default
      return (int)ceil(stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize)/1024);
   }
}

void searchSubdirectories(){}

long int searchFiles(struct dirent * direntp, DIR *dirp, struct stat * stat_buf, flagMask * flags, int oldStdout){
   long int size = 0;

   //search for regular files and symbolic links in current directory
   //TODO: separar em função auxiliar
   while ((direntp = readdir(dirp)) != NULL) {
      char *pathname; //para guardar o path de cada ficheiro ou subdiretório

      pathname = malloc(strlen(flags->path) + 1 + strlen(direntp->d_name) + 1);

      if (pathname == NULL) 
         error_sys("Memory Allocation error\n");
   
      //guarda o path do ficheiro
      sprintf(pathname, "%s/%s", flags->path, direntp->d_name);

      if(getStatus(flags->L,stat_buf,pathname)){
         fprintf(stderr, "Stat error in %s\n", pathname);
         exit(ERROR);
      }
   
      if (S_ISREG(stat_buf->st_mode) || S_ISLNK(stat_buf->st_mode)){
         size += dirFileSize(flags,stat_buf,pathname,oldStdout);
      } 

      free(pathname);  
   }

   return size;
}

