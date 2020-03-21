#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include "simpledu.h"
#include <math.h>
#include <sys/stat.h>

int validatePaths(char** path,char * stringPaths){

   struct stat stat_buf;

   if(strcmp(stringPaths, "") == OK){
      strcpy(path[0],".");
      return OK;
   }//checked

   int  j = 0, k = 0;
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      if(stringPaths[i] == ' '||stringPaths[i] == '\0'){
         path[i][j] = '\0';
         k++;  
         j = 0;
      }
      else{
         path[k][j] = stringPaths[i];
         j++;
      }
   }
   
   char ** tempPath = (char**)malloc(MAX_NUM_PATHS*sizeof(char*));
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      tempPath[i] = (char*)malloc(sizeof(char)*(MAX_PATH+1));
      memset(tempPath[i], 0, sizeof(char)*(MAX_PATH+1));
   }

   j = 0;
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      if (lstat(path[i], &stat_buf) == OK){
         strcpy(tempPath[j],path[i]);
         j++;
      }
   }

   if(strcmp(tempPath[0],"") == OK) return ERRORARGS;

   for(int i = 0; i < MAX_NUM_PATHS; i++)
      memset(path, 0, sizeof(char)*MAX_PATH);

   memcpy(path,tempPath,MAX_NUM_PATHS*MAX_PATH*sizeof(char));
   
   
   free(tempPath);
   
   return OK;   
}

int checkArgs(int argc, char* argv[], flagMask *flags){
   
   int c;
   int option_index = 0;

   static flagMask tempFlags;
   tempFlags.l = 1;

   /* DEBUGGING ...*/
   printf("###########################\n");

   printf("TESTING...\n");

   printf("a: %d\n",tempFlags.a);

   printf("b: %d\n",tempFlags.b);

   printf("B: %d\n",tempFlags.B);

   printf("l: %d\n",tempFlags.l);

   printf("L: %d\n",tempFlags.L);

   printf("S: %d\n",tempFlags.S);

   printf("max-depth: %d",tempFlags.d);
   if(tempFlags.d)
      printf(" value=%d",tempFlags.N);
   printf("\n");

   printf("size: %s\n",tempFlags.size);

   printf("path: %s\n",tempFlags.path);

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

      c = getopt_long(argc, argv, "abB:lLSd:",long_options, &option_index);

      if (c == -1){
         break;
      }

      switch (c) {

         case 0:
            //printf("option %s", long_options[option_index].name);
            //if (optarg)
                  //printf(" with arg %s", optarg);
            //printf("\n");
            break;
         case 'a':
            // printf("option a\n");
            tempFlags.a = 1;
            break;

         case 'b':
            printf("option b\n");
            tempFlags.b = 1;
            break;

         case 'B':
            // printf("option (B) block-size with value '%s'\n", optarg);
            tempFlags.B = 1;
            strcpy(tempFlags.size,optarg);
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
            //printf("Exiting...\n");
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
         sprintf(tempFlags.path + strlen(tempFlags.path),"%s ",argv[optind++]);
      }
      // printf("\n");
      //printf("PATH IS %s\n",tempFlags.path);
   }

   flags->a = tempFlags.a;
   flags->b = tempFlags.b;
   flags->B = tempFlags.B;
   flags->l = 1;
   flags->L = tempFlags.L;
   flags->S = tempFlags.S;
   flags->d = tempFlags.d;
   flags->N = tempFlags.N;

   if(strcmp(tempFlags.size,"") == 0)
      strcpy(flags->size,"1");
   else
      strcpy(flags->size, tempFlags.size);
   
   strcpy(flags->path, tempFlags.path);
   
   return OK;
}

int main(int argc, char* argv[],char* envp[]){

   flagMask flags;
   struct stat stat_buf;
   char ** paths = (char**)malloc(MAX_NUM_PATHS*sizeof(char*));;
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      paths[i] = (char*)malloc(sizeof(char)*(MAX_PATH+1));
      memset(paths[i], 0, sizeof(char)*(MAX_PATH+1));
   }

   if(checkArgs(argc,argv,&flags) != OK){
      printf("Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(ERRORARGS);
   }

   /* DEBUGGING ...*/

   printf("###########################\n");

   printf("RUNNING...\n");

   printf("a: %d\n",flags.a);

   printf("b: %d\n",flags.b);

   printf("B: %d\n",flags.B);

   printf("l: %d\n",flags.l);

   printf("L: %d\n",flags.L);

   printf("S: %d\n",flags.S);

   printf("max-depth: %d",flags.d);
   if(flags.d)
      printf(" value=%d",flags.N);
   printf("\n");

   printf("size: %s\n",flags.size);

   printf("path: %s\n",flags.path);

   printf("###########################\n");

   /*...*/

   if(validatePaths(paths,flags.path) != OK) exit(ERRORARGS);

   for(int i = 0; i < MAX_NUM_PATHS && strcmp(paths[i],"") != OK; i++){
      strcpy(flags.path,paths[i]);
      if(lstat(flags.path,&stat_buf)){
         fprintf(stderr, "Stat error in %s\n", flags.path);
         return 1;
      }
   }

   free(paths);
   exit(OK);
}

