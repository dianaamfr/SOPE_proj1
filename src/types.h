#define OK 0
#define ERROR 1
#define ERRORARGS 1
#define ERROR_BSIZE -1

#define READ 0
#define WRITE 1

#define MAXLINE 60 
#define MAX_PATH 100 
#define MAX_NUM_PATHS 5 
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
  long int size;                            /*< size                -> size of the block in bytes */
  int  L;                                   /*< -L, --dereference   -> dereference all symbolic links */
  int  S;                                   /*< -S, --separate-dirs -> for directories: do not include size of subdirectories */
  int  d;                                   /*< -d, --max-depth     -> print the total for a directory (or file, with --all) */
  int  N;                                   /*< N                   -> only if it is N or fewer levels below the command line argument; */
  char path[MAX_PATH];                      /*< pathname */
} flagMask;
