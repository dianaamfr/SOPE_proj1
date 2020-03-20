/**
 * @file simpledu.h
 * @author My Self
 * @date 20 March 2020
 * @brief File containing simpledu headers and constants
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * @see http://fnch.users.sourceforge.net/doxygen_c.html
 * @see https://wiki.scilab.org/Doxygen%20documentation%20Examples
 * for more information.
 */

#ifndef SIMPLEDU
#define SIMPLEDU

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

/** 
 * @brief Build flagMask struct (set active/inactive flags and path)
 * By checking the arguments passed to the function, it activates the @p flags  
 * @param argc number of args passed
 * @param argv args passed
 * @param flags flagMask to be built
 * @return true if successful, false otherwise
 */
int checkArgs(int argc,char* argv[],flagMask *flags);

/**
 * @brief Main function for simpledu command 
*/
int main(int argc, char* argv[],char* envp[]);

#endif