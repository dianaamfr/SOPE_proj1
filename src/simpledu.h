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
 * for more information.
 */

#ifndef SIMPLEDU
#define SIMPLEDU

#define MAX_PATH 100 
#define MAX_BLOCK_STR_SIZE 10 

/**
 * @brief Mask to save the active flags of simpledu.
 *
 * Firstly instantiated in main function, it will save the user options
 * as well as the current path being search at any moment.
 */
typedef struct flagMask{
  unsigned int  l;                      /*< -l, --count-links */
  unsigned int  a;                      /*< -a, --all           -> write counts for all files, not just directories */
  unsigned int  b;                      /*< -b --bytes          -> show real size of data in bytes;*/
  unsigned int  B;                      /*< -B, --block-size    -> defines the size of each block (size) */
  char size[MAX_BLOCK_STR_SIZE];        /*< -size               -> size of the block in bytes */
  unsigned int  L;                      /*< -L, --dereference   -> dereference all symbolic links */
  unsigned int  S;                      /*< -S, --separate-dirs -> for directories: do not include size of subdirectories */
  int  max_depth;                       /*< --max-depth=N       -> print the total for a directory (or file, with --all) only if it is N or fewer levels below the command line argument; */
  char path[MAX_PATH];                  /*< pathname */
} flagMask;

/** Fill Flag Mask with default values */

/**
 * @brief Fill Flag Mask with the Default Values.
 * @p flags.l is always active (1), 
 * @p flags.path is empty and all the other flags are inactive (0),
 * except @p flags.max_depth which gets a value of -1, because if no pathname was given, 
 * the total of all directories should be shown (no level limit was specified).
 * @param flags flagMask to be filled with the Default Values.
 * @see flagMask
 * @note Something to note.
 */
void defaultFlags(flagMask * flags);

/** 
 * @brief Check if the argument is a flag
 * @param arg Needs further explanation...
 * @return true if it is a flag, false otherwise
 * */
bool isFlag(char arg);

/** @brief Validate the format of sizing options
 * This is only applicable to the "--max-depth" and "-B" options.
 * With option "-B SIZE", SIZE can have a numeric value greater than 0 alone, 
 * or followed by a unit symbol (K,M,G,T,P,E,Z,Y).
 * With option "--max-depth N", N is need to be a number.
 * @param arg Needs further explanation
 * @return true if it is a flag, false otherwise
*/
bool validSize(char size[]);

/**
 * @brief Activates the corresponding flags from flagMask @p flags
 * If the option is valid, activate the corresponding flag.
 * @param flags flagMask to be modified
 * @param flag char representing the corresponding flag
 * @param n Empty if the flag doesn't have numeric info. For --max-depth and -B it must have some value.
 * @return true if successful, false otherwise
 * @note The value of n must be valid (checked with validSize before calling this function).
 */
bool activateFlag(flagMask *flags, char flag[], char n[]);

/** 
 * @brief Build flagMask struct (set active/inactive flags and path)
 * By checking the arguments passed to the function, it activates the @p flags  
 * @param argc number of args passed
 * @param argv args passed
 * @param flags flagMask to be built
 * @return true if successful, false otherwise
 */
bool checkArgs(int argc,char* argv[],flagMask *flags);

/**
 * @brief Main function for simpledu command 
*/
int main(int argc, char* argv[],char* envp[]);

#endif