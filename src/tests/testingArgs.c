#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h> 
#include <string.h>

#define OK 0
#define ERRORARGS 1

#define MAX_PATH 100 
#define MAX_BLOCK_STR_SIZE 10 

/**
 * @brief Mask to save the active flags of simpledu.
 *
 * Firstly instantiated in main function, it will save the user options
 * as well as the current path being search at any moment.
 */
typedef struct flagMask{
  int  l;                                   /*< -l, --count-links */
  int  a;                                   /*< -a, --all           -> write counts for all files, not just directories */
  int  b;                                   /*< -b --bytes          -> show real size of data in bytes;*/
  int  B;                                   /*< -B, --block-size    -> defines the size of each block (size) */
  char size[MAX_BLOCK_STR_SIZE];            /*< size                -> size of the block in bytes */
  int  L;                                   /*< -L, --dereference   -> dereference all symbolic links */
  int  S;                                   /*< -S, --separate-dirs -> for directories: do not include size of subdirectories */
  int  d;                                   /*< -d, --max-depth     -> print the total for a directory (or file, with --all) */
  int  N;                                   /*< N                   -> only if it is N or fewer levels below the command line argument; */
  char path[MAX_PATH];                      /*< pathname */
} flagMask;

int main(int argc, char *argv[]) {
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

      if (c == -1)
         break;

      switch (c) {

         case 0:
               printf("option %s", long_options[option_index].name);
               if (optarg)
                  printf(" with arg %s", optarg);
               printf("\n");
               break;

         case 'a':
               printf("option a\n");
               tempFlags.a = 1;
               break;

         case 'b':
               printf("option b\n");
               tempFlags.b = 1;
               break;

         case 'B':
               printf("option (B) block-size with value '%s'\n", optarg);
               tempFlags.B = 1;
               strcpy(tempFlags.size,optarg);
               break;

         case 'l':
               printf("option l\n");
               tempFlags.l = 1;
               break;

         case 'L':
               printf("option L\n");
               tempFlags.L = 1;
               break;

         case 'S':
               printf("option S\n");
               tempFlags.S = 1;
               break;
         
         case 'd':
               printf("option (d) max-depth with value '%s'\n", optarg);
               tempFlags.d = 1;
               tempFlags.N = atoi(optarg);
               break;

         case '?':
               /* getopt_long already printed an error message. */
               printf("Exiting...\n");
               return ERRORARGS;
               break;

         default:
               printf("Error: getopt returned character code 0%o \n", c);
               return ERRORARGS;
               break;
        }
   }

   if (optind < argc) {
      printf("non-option ARGV-elements: ");
      while (optind < argc){
         printf("%s ", argv[optind]);
         sprintf(tempFlags.path + strlen(tempFlags.path),"%s ",argv[optind++]);
      }
      printf("\n");
      printf("PATH IS %s\n",tempFlags.path);
   }

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
   
   return OK;
}