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
 * @see https://wiki.scilab.org/Doxygen%20documentation%20Examples for more information.
 * @note Credits to the "Example of Parsing Long Options with getopt_long"
 * @see checkArgs
 */

#include <sys/stat.h>
#include "types.h"

#ifndef SIMPLEDU
#define SIMPLEDU

/** 
 * @brief Build flagMask struct (set active/inactive flags and path)
 * By checking the arguments passed to the function, it activates the @p flags  
 * @param argc number of args passed
 * @param argv args passed
 * @param flags flagMask to be built
 * @return OK if successful, ERRORARGS otherwise
 * @note Lots of Credits to those who managed to give a very good explanation at 
 * @see https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
 * @see https://linux.die.net/man/3/getopt_long for more info on the functions used
 */
int checkArgs(int argc, char* argv[], flagMask *flags);

/**
 * @brief Check if the path exists
 * @param path the name of the path
 * @return OK if path exists, ERRORARGS otherwise
*/
int validatePath(char * path);

/**
 * @brief Check if the size B
 * @param oparg size of B
 * @return OK if B SIZE is valid, ERROR_BSIZE otherwise
*/
long int checkBsize(char *optarg);

/**
 * @brief Show active flags on screen for testing purposes
 * @param flags flagMask to print
 * @param description describes the purpose for showing the flags on the creen
*/
void printFlags(flagMask * flags, char * description);

/**
 * @brief Calculate the size occupied in disk by a single regular file
 * Used when the user only asks for the size of a single file
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @return the size occupied by the file
*/
long int regularFileSize(flagMask *flags, struct stat *stat_buf);

/**
 * @brief Calculate the size occupied in disk by a single symbolic link
 * Used when the user only asks for the size of a single symbolic link
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @return the size occupied by the file
*/
long int symbolicLinkSize(flagMask *flags, struct stat *stat_buf);

/**
 * @brief Calculate the size of a directory file and show it on the screen if --all is specified
 * Used to process each file of a directory and return its size
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @param pathname the name of the path of the file
 * @return the size occupied by the file
*/
long int dirFileSize(flagMask *flags, struct stat *stat_buf, char * pathname);

/**
 * @brief Main function for simpledu command 
*/
int main(int argc, char* argv[], char* envp[]);

#endif