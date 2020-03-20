#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "simpledu.h"

void defaultFlags(flagMask * flags){
   flags->a = flags->b = flags->B = flags->L = flags->S = 0;   /*flags are not active*/
   flags->l = 1;                                               /*-l is always active*/
   flags->max_depth = -1;                                      /*if no pathname was given, the total of all directories should be shown (no level limit was specified)*/
}

bool isFlag(char arg){
   return arg == '-';
}

bool validSize(char size[]){
   /*TODO*/
   if(isFlag(size[0])) return false;
   return true;
}

bool activateFlag(flagMask *flags, char flag[], char n[]){
   char temp[12];
   switch(flag[0]){
      case 'l':
         /*Depende do que o prof disser. Pode começar inativa e no caso de ter o -l ativa-se a flag tal como nas outras flags (Para já aceita sem -l)*/
         break;
      case 'a':
         if(flags->a == 0) flags->a = 1; /*if flag is not active already activate it*/
         else return false;
         break;
      case 'b':
         if(flags->b == 0) flags->b = 1;
            else return false;
            break;
      case 'B':
         if(flags->B == 0){
            flags->B = 1;
            strcpy(flags->size,n);
         }
         else return false;
         break;
      case 'L':
         if(flags->L == 0) flags->L = 1;
         else return false;
         break;
      case 'S':
         if(flags->S == 0) flags->S = 1;
         else return false;
         break;
      case '-':
         memcpy(temp,&flag[1],10);
         if(strcmp(temp,"max-depth=") == 0) 
            flags->max_depth = atoi(n);
         else return false;
         break;
      default:
         printf("default");
         return false;
   }
   return true;
}

bool checkArgs(int argc,char* argv[],flagMask *flags){
   defaultFlags(flags);

   for(int i = 1; i < argc; i++){ /*its a */
      if(isFlag(argv[i][0])){ /*we have a flag*/
         char temp[13];
         memcpy(temp,argv[i],12);
         
         if(strlen(argv[i]) == 2 && strcmp(argv[i],"-B") != 0){ /*if the option is not "-B" or "--max-depth"*/
            if(!activateFlag(flags,&argv[i][1],"")) return false; /*invalid flag*/
         }
         else if(strlen(argv[i]) == 2 && strcmp(argv[i],"-B") == 0 && i+1 < argc){ /*if the option is "-B" and then SIZE is in the next argument (-B SIZE)*/
            if(!validSize(argv[i+1])) return false;
            if(!activateFlag(flags, &argv[i][1],argv[i+1])) return false; /*invalid flag*/
            i++; /*the next argument is part of this flag/option and has already been considered*/
         }
         else if(strcmp(temp,"--max-depth=") == 0){ /*if the option is "--max-depth=N"*/
            if(!validSize(&argv[i][12])) return false;
            if(!activateFlag(flags, &argv[i][1],&argv[i][12])) return false; /*invalid flag*/
         }
         else return false;
      }
      else return false; /*Check if the path exists*/
   }
   return true;
   //printFlags(flags);
}

int main(int argc, char* argv[],char* envp[]){
   flagMask flags;
   if(!checkArgs(argc,argv,&flags)){
      printf("Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(1);
   }
   printf("RUNNING... %d\n", flags.l);

   /*...*/
   exit(0);
}

