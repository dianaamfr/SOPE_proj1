/**
 * @file aux.h
 * @author My Self
 * @date 20 March 2020
 * @brief File containing simpledu headers and constants
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 */

#ifndef AUX_H
#define AUX_H

#include "types.h"
#include <sys/stat.h>

/**
 * @brief Check if the path exists
 * @param path the name of the path
 * @return OK if path exists, ERRORARGS otherwise
*/
int validatePath(char * path);

/**
 * @brief Show active flags on screen for testing purposes
 * @param flags flagMask to print
 * @param description describes the purpose for showing the flags on the creen
*/
void printFlags(flagMask * flags, char * description);

/**
 * @brief Calculate the size of a directory file and show it on the screen if --all is specified
 * Used to process each file of a directory and return its size
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @param pathname the name of the path of the file
 * @param stdout_fd the file descriptor to use when writing to the console
 * @return the size occupied by the file
*/
long int dirFileSize(flagMask *flags, struct stat *stat_buf, char * pathname, int stdout_fd);

#endif
