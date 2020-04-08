/**
 * @file utils.h
 * @author SOPE T02G05 
 * @date 20 March 2020
 * @brief File containing utilities' headers and constants
 */

#ifndef AUX_H
#define AUX_H

#include <sys/stat.h>
#include <dirent.h>
#include "types.h"

/**
 * @brief Print an error message @p msg to stderr
 * @param msg error message to be printed
*/
void error_sys(char * msg);

/**
 * @brief Block SIGUSR2
 * @return OK if no error occurred, ERROR otherwise
*/
void blockSIGUSR2();

/**
 * @brief Check for pending SIGUSR2
 * SIGUSR2 is sent by a child process to itself.
 * Used to know when the parent process should overwrite 
 * the environment variable if it exists.
 * The only process that should not have pending 
 * signals when it starts running is the first process, 
 * the one that runs on the first directory.
 * @return OK if no error occurred, ERROR otherwise
*/
int pendingSIGUSR2();

/**
 * @brief Print active flags
 * @param flags flagMask to be printed
 * @param description with a simple message about the reason for printing the flags
 * @note For Debugging Purposes
*/
void printFlags(flagMask * flags, char * description);

/**
 * @brief Gets @p flags as a string
 * See code for format details
 * @param flags flagMask to be converted to a string
 * @param str the returning formatted output from the flagMask @p flag
 * @note For Debugging Purposes AND logRECV_PIPE or logSEND_PIPE
*/
void flagsToString(flagMask * flags, char * str);

/**
 * @brief Check if the current process is a subprocess
 * Checks the value of the environment variable which stores the pid of the parent process
 * @return OK if it is a subprocess, ERROR otherwise
*/
int isChildProcess();

/**
 * @brief Get the pid of the parent of all processes
 * @return The pid of the parent of all processes
*/
int getParentPid();

/**
 * @brief Set an environment variable with the pid of the parent of all processes
*/
void setParentPid();

/**
 * @brief Wait for unfinished subprocesses before exiting current process
*/
void waitForSubprocesses();

/**
 * @brief Signal Handler for assigned signals SIGINT and SIGUSR1
 * Only those two have this handler attached, but some more have a part in the job
 * Naming them all: 
 * SIGINT   - for user interaction
 * SIGSTOP  - for stopping all processess from the process group
 * SIGCONT  - for continuing the work
 * SIGTERM  - for terminating everything
 * It works as simples as this: 
 * User sends SIGINT by pressing Ctrl+C.
 * It is firstly ignored by all processes, except by the parent of all processes.
 * When the parent receives the SIGINT, it sends a SIGSTOP
 * to the group where all other subprocesses now reside.
 * The parent, then, waits for the user decision of continuing, 
 * sending a SIGCONT to all, or terminating, sending a SIGTERM.
 * @param signo signal received
*/
void sigHandler(int signo);


/**
 * @brief Signal Handler for assigned signal SIGUSR1
 * Only used by the main parent process to kill all the subprocesses
 * if a string size memory error was detected when reading paths. 
 * This is done by sending a SIGTERM to all subprocesses and finally to itself.
 * @param signo signal received
*/
void sigUSR1Handler(int signo);

/**
 * @brief Attaches the @p handler of the signal @p SIG to the current process
 * @param action struct for attaching the handler and registering it
 * @param SIG signal to be assigned with the @p handler
 * @param handler prototyped Handler function to be attached to the signal
*/
void attachSIGHandler(struct sigaction action, int SIG, __sighandler_t handler);

/**
 * @brief Check if the @p do not exceeds the LIMIT_PATH size
 * @param path path to be checked
 * @return OK if not exceeded, ERRORARGS otherwise
*/
int validPathSize(char * path);

/**
 * @brief Check if the size B is valid
 * @param optarg size B to be checked
 * @return OK if B SIZE is valid, ERROR_BSIZE otherwise
*/
long int checkBsize(char * optarg);

/** 
 * @brief Build flagMask struct (set active/inactive flags and path)
 * It activates the @p flags, by checking the arguments passed to the function  
 * @param argc number of args passed
 * @param argv args passed
 * @param flags flagMask to be built
 * @return OK if successful, ERRORARGS otherwise
 * @note Lots of Credits to those who managed to give a very good explanation at 
 * @see https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
 * @see https://linux.die.net/man/3/getopt_long for more info on the functions used
 */
int checkArgs(int argc, char * argv[], flagMask * flags);

/**
 * @brief Check if the @p path is referring the current directory and remove duplicate '/' if it is
 * @param path current path to be checked
*/
void removeDuplicateBar(char * path);

/**
 * @brief Check if the @p path exists
 * @param path current path to be checked
 * @return OK if path exists, ERRORARGS otherwise
*/
int validatePath(char * path);

/**
 * @brief Getting file status with lstat, if -L flag is inactive, or with stat, if -L flag is active
 * @param flag_L the -L flag from the flagMask
 * @param stat_buf where the info about the file is returned
 * @param path the path to the file
 * @return OK if no errors occurred, ERROR otherwise
*/
int getStatus(int flag_L, struct stat * stat_buf, char * path);

/**
 * @brief Gets the size of one directory (without counting files/subdirectories)
 * @param flags_b b flag
 * @param stat_buf where resides the info about the current directory
 * @return the size of the current directory
*/
int currentDirSize(int flags_b, struct stat * stat_buf);

/**
 * @brief Gets the size of the subdirectories in one directory
 * @param dirp directory stream 
 * @param flags flagMask with the active flags
 * @param stdout_fd the descriptor to be use for printing the info
 * @return total size of the current directory
*/
long int searchSubdirs(DIR * dirp, flagMask * flags, int stdout_fd);

/**
 * @brief Gets the size of one subdirectory
 * Creates a child process to execute simpledu 
 * on the subdirectory being currently searched.
 * Uses forks, pipes and execs to do it.
 * @param stdout_fd the old descriptor to be sent to the new process
 * @param flags flagMask with active flags
 * @param subDirPath the path of the subdirectory to be processed
 * @return the size of the subdirectory
*/
long int processSubdir(int stdout_fd, flagMask * flags, char * subDirPath);

/**
 * @brief Gets the size of the files in the current directory
 * @param dirp directory stream 
 * @param stat_buf where resides the info about the current directory
 * @param flags flagMask with active flags
 * @param stdout_fd the descriptor to be use for printing the info
 * @return the size of the files present in the current directory
*/
long int searchFiles(DIR * dirp, flagMask * flags, int stdout_fd);

/**
 * @brief Calculates the size of a directory file and prints it if --all is specified
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @param pathname the path of the file
 * @param stdout_fd the descriptor to be use for printing the info
 * @return the size of the file
*/
long int dirFileSize(flagMask * flags, struct stat * stat_buf, char * pathname, int stdout_fd);

/**
 * @brief Converts the size in system blocks to blocks with size_b
 * @param bytesSize the size to be converted
 * @param blockSize the size of the blocks
 * @return size in blocks of size_b
*/
long int sizeInBlocks(long int bytesSize, long int blockSize);

/**
 * @brief Calculates the size of a single regular file
 * Used when the user only asks for the size of a single file
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @return the size of the file
*/
long int regularFileSize(flagMask * flags, struct stat * stat_buf);

/**
 * @brief Calculates the size of a single symbolic link
 * Used when the user only asks for the size of a single symbolic link
 * @param flags flagMask with active flags
 * @param stat_buf with info about block size and file size in bytes
 * @return the size of the symbolic link
*/
long int symbolicLinkSize(flagMask * flags, struct stat * stat_buf);

/**
 * @brief Print the size of a file
 * @param flags flagMask with active flags
 * @param size file size
 * @param pathname the file path
 * @param stdout_fd the descriptor to be used for printing the info
*/
void printFileInfo(flagMask * flags, long int size, char * pathname, int stdout_fd);

/**
 * @brief Print the size of a directory
 * @param flags flagMask with active flags
 * @param size file size
 * @param stdout_fd the descriptor to be used for printing the info
*/
void printDirInfo(flagMask * flags, long int size, int stdout_fd);

#endif
