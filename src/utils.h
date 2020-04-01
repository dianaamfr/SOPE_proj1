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
#include <dirent.h>

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
 * @brief Check if the size B
 * @param oparg size of B
 * @return OK if B SIZE is valid, ERROR_BSIZE otherwise
*/
long int checkBsize(char *optarg);

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
 * @param stdout_fd the file descriptor to use when writing to the console
 * @return the size occupied by the file
*/
long int dirFileSize(flagMask *flags, struct stat *stat_buf, char * pathname, int stdout_fd);

/**
 * @brief Print error message to stderr
 * @param msg the error message to print
*/
void error_sys(char *msg);

/**
 * @brief Get file status with lstat() if L flag is inactive or stat() if L flag is active
 * @param flag_L L flag
 * @param stat_buf where the info about the file is returned
 * @param path the path of the file
 * @return OK if no problems occured, ERROR otherwise
*/
int getStatus(int flag_L, struct stat * stat_buf, char * path);

/**
 * @brief convert the size in system blocks to blocks with Bsize
 * @param totalSize the size to convert
 * @param Bsize the size of the blocks
 * @return size in blocks of Bsize
*/
double sizeInBlocks(long int totalSize, long int Bsize);

/**
 * @brief sum the size of one directory (without files/subdirectories)
 * @param flags_B B flag
 * @param flags_b b flag
 * @param stat_buf where the info about the directory is returned
 * @return OK if no problems occured, ERROR otherwise
*/
int currentDirSize(int flags_B, int flags_b, struct stat * stat_buf);

/**
 * @brief sum the size of the files in one directory
 * @param dirp directory stream 
 * @param stat_buf where the info about the directory is stored
 * @param flags flagMask with active flags
 * @param oldStdout the descriptor to be use to print the sizes to the console
 * @return the size of the subdirectories in the directory pointed by dirp
*/
long int searchFiles(DIR *dirp, flagMask * flags, int oldStdout);

/**
 * @brief sum the size of the subdirectories in one directory
 * @param dirp directory stream 
 * @param stat_buf where the info about the directory is stored
 * @param flags flagMask with active flags
 * @param oldStdout the descriptor to be use to print the sizes to the console
 * @return
*/
long int searchSubdirs(DIR *dirp, flagMask * flags, int stdout);

/**
 * @brief get the size of one subdirectory
 * Uses fork() to create a new process, exec() to execute the simpledu for that process and pipe() to create
 * 2 pipes, one to send the flags to the subdirectory process and other toreceive the size of the subdirectory
 * @param stdout the descriptor to be sent to the new process
 * @param flags flagMask with active flags
 * @param subDirPath the path of the subdirectory to process
 * @return the size of the subdirectory
*/
long int processSubdir(int stdout, flagMask * flags, char * subDirPath);

/**
 * @brief Block USR1 signal
 * @return OK if no problems occured, ERROR otherwise
*/
void blockSIGUSR1();

/**
 * @brief Check for pending USR1 signals
 * USR1 is sent by a child process to itself -> the only process that should not have pending 
 * signals when it starts running is the first process to run on the first directory
 * @return OK if no problems occured, ERROR otherwise
*/
int pendingSIGUSR1();


#endif
