#include "types.h"
#include <sys/stat.h>

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
 * @brief Main function to search in a directory
*/
int main(int argc, char* argv[], char* envp[]);