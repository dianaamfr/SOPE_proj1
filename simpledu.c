#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PATH 100 
#define MAX_BLOCK_STR_SIZE 10 

/**Mask to save the active flags of simpledu command and the path which is actually being searched*/
typedef struct flagMask{
  unsigned int  l;         /*-l, --count-links*/
  unsigned int  a;         /*-a, --all -> write counts for all files, not just directories*/
  unsigned int  b;         /*-b --bytes -> show real size of data in bytes;*/
  unsigned int  B;         /*-B, --block-size -> defines the size of each block (size))*/
  char size[MAX_BLOCK_STR_SIZE];      /*-size -> size of the block in bytes*/
  unsigned int  L;         /*-L, --dereference -> dereference all symbolic links*/
  unsigned int  S;         /*-S, --separate-dirs -> for directories do not include size of subdirectories*/
  int  max_depth; /*--max-depth=N -> print the total for a directory (or file, with --all) only if it is N or fewer levels below the command line argument;*/
  char path[MAX_PATH];             /*pathname*/
} flagMask;

/** Fill Flag Mask with default values "l" is active (1), "max_depth" is not active (-1), "path" is empty and all the other flags are inactive (0) */
void defaultFlags(flagMask * flags){
   flags->a = flags->b = flags->B = flags->L = flags->S = 0; /*flags are not active*/
   flags->l = 1; /*-l is always active*/
   flags->max_depth = -1; /*if no pathname was given, the total of all directories should be shown (no level limit was specified)*/
}

/** Check if the argument is a flag*/
bool isFlag(char arg){
   return arg == '-';
}

/** Validate the format of the numeric value in "--max-depth" and "-B" options
 * 
 * in "-B SIZE" SIZE can have: a numeric value > 0 or a numeric value > 0 followed by a unit (K,M,G,T,P,E,Z,Y)
 * in "--max-depth N" N is a number
*/
bool validSize(char size[]){
   /*TODO*/
   return true;
}

/** If the option is valid activate its flag and return true. Otherwise return false; n = "" if the flag doesn't have numeric info. For --max-depth and -B it must be filled
 * The value of n must be valid (check with "validSize()" before calling this function)
*/
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
         return false;
   }
   return true;
}

/** Build flagMask struct (set active/inactive flags and path) by checking the arguments passed to the function*/
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

   /*...*/
   exit(0);
}