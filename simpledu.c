#include <stdlib.h>
#include <stdio.h>

#define MAX_PATH 100 

/**Mask to save the active flags of simpledu command and the path which is actually being searched*/
typedef struct flagMask{
  unsigned int  l;         /*-l, --count-links*/
  unsigned int  a;         /*-a, --all -> write counts for all files, not just directories*/
  unsigned int  b;         /*-b --bytes -> show real size of data in bytes;*/
  unsigned int  B;         /*-B, --block-size -> defines the size of each block (size))*/
  unsigned int  size;      /*-size -> size of the block in bytes*/
  unsigned int  L;         /*-L, --dereference -> dereference all symbolic links*/
  unsigned int  S;         /*-S, --separate-dirs -> for directories do not include size of subdirectories*/
  int  max_depth; /*--max-depth=N -> print the total for a directory (or file, with --all) only if it is N or fewer levels below the command line argument;*/
  char path[MAX_PATH];             /*pathname*/
} flagMask;

/** Fill Flag Mask with default values "l" is active (1), "max_depth" is not active (-1), "path" is empty and all the other flags are inactive (0) */
void defaultFlags(flagMask * flags){
   flags->a = flags->b = flags->B = flags->size = flags->L = flags->S = 0; /*flags are not active*/
   flags->l = 1; /*-l is always active*/
   flags->max_depth = -1; /*if no pathname was given, the total of all directories should be shown (no level limit was specified)*/
}

/** Check if the argument is a flag*/
bool isFlag(char argv[]){
   /*....*/
   return true;
}

void activateFlag(char argv[]){
   /*....*/
   return true;
}


/** Build flagMask struct (set active/inactive flags and path) by checking the arguments passed to the function*/
bool checkFlags(int argc,char* argv[],flagMask *flags){
   defaultFlags(flags);
   switch(argc){
      case 1: /*simpledu (current path) -> flag active by default*/
         return true;
      case 2: /*simpledu -l (current path) */
         return (argv[1] != "-l" && argv[1] != "--count-links"); /*flag active by default*/
      case 3: //simpledu -l [?]
         if(isFlag(argv[2])){
            activateFlag(argv[2]);
         } else memcpy(path,argv[2],MAX_PATH); /*if not flag it is the path*/
         break;
      default:
         return false;
   }
}

int main(int argc, char* argv[],char* envp[]){
   flagMask flags;

   if(!checkFlags(argc,argv,&flags)){
      printf("Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(1);
   }

   /*...*/
   exit(0);
}