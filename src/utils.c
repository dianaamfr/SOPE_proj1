#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h> 
#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "types.h"
#include "utils.h"
#include "logging.h"

extern pid_t gid;

void blockSIGUSR2(){

   sigset_t mask;

   // Temporarily blocking SIGUSR2
   sigemptyset (&mask);
   sigaddset (&mask, SIGUSR2);

   // Set new mask
   sigprocmask(SIG_BLOCK, &mask, NULL);
}

int pendingSIGUSR2(){

   sigset_t pending_signals;
   int sig;

   // Checking for pending SIGUSR2
   if (sigpending(&pending_signals) == 0 && sigismember (&pending_signals, SIGUSR2)){
      
      sigwait(&pending_signals,&sig);

      return OK;
   }

   return ERROR;
}

void error_sys(char *msg){

   fprintf(stderr,"%s\n",msg);

   logEXIT(ERROR);
   exit(ERROR);
} 

void printFlags(flagMask * flags, char * description){
   
   printf("###########################\n");

   printf("%s...\n", description);

   printf("a: %d\n", flags->a);

   printf("b: %d\n", flags->b);

   printf("B: %d\n", flags->B);

   printf("l: %d\n", flags->l);

   printf("L: %d\n", flags->L);

   printf("S: %d\n", flags->S);

   printf("max-depth: %d", flags->d);
   if (flags->d)
      printf(" value=%d", flags->N);
   printf("\n");

   printf("size: %ld\n",flags->size);

   printf("path: %s\n", flags->path);

   printf("###########################\n");
}

void flagsToString(flagMask * flags, char * str){
   
   double time = flags->startTime.tv_sec * 1000.0 + flags->startTime.tv_usec / 1000.0;
   
   sprintf(str,"%d%d%d%d_%ld_%d%d%d%d_%s_%f",
   flags->l,
   flags->a,
   flags->b,
   flags->B,
   flags->size,
   flags->L,
   flags->S,
   flags->d,
   flags->N,
   flags->path,
   time);
}

int isChildProcess(){
   int pid = getpid();

   if(pid != getParentPid())
      return OK;
   
   return ERROR;
}

int getParentPid(){
   return atoi(getenv("SIMPLEDU_PPID"));
}

void setParentPid(){

   char pidStr[13];
   int pid = getpid();
   sprintf(pidStr, "%d", pid);
   
   setenv("SIMPLEDU_PPID",pidStr,1);
}

void waitForSubprocesses(){
   // If in the parent of all processes wait for all subprocesses
   if(!isChildProcess()){
      int status;
      unsetenv("SIMPLEDU_PPID");
      while(waitpid(-gid,&status, 0) > -1){}
   }
}

void sigHandler(int signo){

   if (signo == SIGINT){ // Ignored by every process except the parent

      logRECV_SIGNAL(SIGINT);

      logSEND_SIGNAL(SIGSTOP, -gid);
      // Sending a SIGSTOP to all child processess 
      kill(-gid,SIGSTOP);  

      char c;
      printf("Continue? (Y or N) ");
      scanf("%c",&c);
      while ((getchar()) != '\n');

      printf("You entered: %c\n", c);
      
      if (c == 'Y'){
         printf("CONTINUING! - %d - %d\n", getpid(), getppid());
         
         logSEND_SIGNAL(SIGCONT,-gid);
         kill(-gid,SIGCONT); // Sending a SIGCONT to all processess
      }
      else{
         printf("TERMINATING! - %d - %d\n", getpid(), getppid());
         
         logSEND_SIGNAL(SIGTERM, -gid);
         kill(-gid, SIGTERM); // Sending a SIGTERM to all processess
         
         unsetenv("SIMPLEDU_PPID");

         logSEND_SIGNAL(SIGTERM, getpid());
         logEXIT(SIGTERM);
         kill(getpid(),SIGTERM);
      }
   }
}

void sigUSR1Handler(int signo){
   if (signo == SIGUSR1) {
      logRECV_SIGNAL(SIGUSR1); 

      fprintf(stderr, "Max Path Size (%d) exceeded\n", LIMIT_PATH);
      
      logSEND_SIGNAL(SIGTERM, -gid);
      killpg(gid, SIGTERM); // Sending a SIGTERM to all processess

      unsetenv("SIMPLEDU_PPID");   

      logSEND_SIGNAL(SIGTERM, getpid());
      logEXIT(SIGTERM);
      kill(getpid(),SIGTERM);
   }
}

void attachSIGHandler(struct sigaction action, int SIG, __sighandler_t handler){
   
   action.sa_handler = handler;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;

   if (sigaction(SIG, &action, NULL) < 0){
      fprintf(stderr,"Unable to install SIG handler\n");
      logEXIT(ERROR);
      exit(ERROR);
   }
}

int validPathSize(char * path){
   return !( strlen(path) < LIMIT_PATH );
}

long int checkBsize(char * optarg) {

   int alphabet = 0, number = 0;
   
   for (int i = 0; optarg[i] != '\0'; i++){

      if (isalpha(optarg[i])) 
         alphabet++;
      else if (isdigit(optarg[i])) 
         number++;
   }

   if (alphabet != 0 && number != 0) 
      return ERROR_BSIZE;
   
   else if (number > 0 && alphabet == 0 && strcmp(optarg, "0") != 0) 
      return atoi(optarg);

   else if (alphabet == 1 && number == 0) {
      if (strcmp(optarg, "K") == 0) return 1024;
      else if (strcmp(optarg, "M") == 0) return pow(1024,2);
      else if (strcmp(optarg, "G") == 0) return pow(1024,3);
      else if (strcmp(optarg, "T") == 0) return pow(1024,4);
      else if (strcmp(optarg, "P") == 0) return pow(1024,5);
      else if (strcmp(optarg, "E") == 0) return pow(1024,6);
      else return ERROR_BSIZE;
   }
   else if (alphabet == 2 && number == 0) {
      if (strcmp(optarg, "KB") == 0) return 1000;
      else if (strcmp(optarg, "MB") == 0) return pow(1000,2);
      else if (strcmp(optarg, "GB") == 0) return pow(1000,3);
      else if (strcmp(optarg, "TB") == 0) return pow(1000,4);
      else if (strcmp(optarg, "PB") == 0) return pow(1000,5);
      else if (strcmp(optarg, "EB") == 0) return pow(1000,6);
      else return ERROR_BSIZE;
   }
   else return ERROR_BSIZE;

   return OK;
}

int checkArgs(int argc, char * argv[], flagMask * flags){
   
   int c;
   int option_index = 0;
   long int size_b;

   static flagMask tempFlags;
   tempFlags.l = 1;

   /* DEBUGGING ...*/
   // printFlags(&tempFlags, "TESTING");
   /*...*/
   
   while(1) {         
      
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

      // The short_options array, as a string, is interpreted 
      // as a no_argument option for every alone character and 
      // a required_argument option for every character followed by a colon (:)
      c = getopt_long(argc, argv, "abB:lLSd:", long_options, &option_index);
      // c contains the current arg from argv that is being analyzed

      if (c == -1)
         break;
      
      switch (c){

         case 0: // This is a default option as found in the legit Linux Man Pages source 
            // printf("option %s", long_options[option_index].name);
            // if (optarg)
               // printf(" with arg %s", optarg);
            // printf("\n");

            break;

         case 'a':
            // printf("option a\n");
            tempFlags.a = 1;

            break;

         case 'b':
            // printf("option b\n");
            tempFlags.b = 1;

            if(tempFlags.B){ // If -B SIZE ... -b assume -b (the last option is assumed) => disable -B 
               tempFlags.size = 0;
               tempFlags.B = 0;
            }

            break;
         case 'B':
            // printf("option (B) block-size with value '%s'\n", optarg);
            tempFlags.B = 1;
            size_b = checkBsize(optarg);

            if (size_b == ERROR_BSIZE){
               fprintf(stderr, "%s: invalid -B argument: '%s'\n",argv[0],optarg);
               return ERRORARGS;
            }

            if (size_b == 1 && tempFlags.b){ //if -B 1 and -b 1, then assume -b (order is irrelevant)
               tempFlags.size = 0;
               tempFlags.B = 0;
            }
            else 
               tempFlags.size = size_b; //if -b is not active 

            break;

         case 'l':
            // printf("option l\n");
            tempFlags.l = 1;

            break;

         case 'L':
            // printf("option L\n");
            tempFlags.L = 1;

            break;

         case 'S':
            // printf("option S\n");
            tempFlags.S = 1;

            break;
         
         case 'd':
            // printf("option (d) max-depth with value '%s'\n", optarg);
            tempFlags.d = 1;
            tempFlags.N = atoi(optarg);

            break;

         case '?': // Unkown option in argv
            /* getopt_long already printed an error message. */
            // printf("Exiting...\n");
            return ERRORARGS;

            break;

         default: // Some other error may have occurred 

            printf("Error: getopt returned character code 0%o \n", c);
            return ERRORARGS;

            break;
        }
   }

   // This reveals non-option args, like loose strings
   // In our case, it may represent the path
   if (optind < argc){
      // printf("non-option ARGV-elements: ");
      while (optind < argc){
         // printf("%s ", argv[optind]);
         sprintf(tempFlags.path, "%s", argv[optind++]);
         //sprintf(tempFlags.path + strlen(tempFlags.path), "%s ", argv[optind++]);  //in case we want multiple files
      }
      // printf("\n");
      // printf("PATH IS %s\n", tempFlags.path);
   }

   if ( validPathSize(tempFlags.path) != OK ){
      fprintf(stderr, "Max Path Size (%d) exceeded\n", LIMIT_PATH);
      return ERRORARGS;
   }

   flags->a = tempFlags.a;
   flags->b = tempFlags.b;
   flags->B = tempFlags.B;
   flags->l = 1;
   flags->L = tempFlags.L;
   flags->S = tempFlags.S;
   flags->d = tempFlags.d;
   flags->N = tempFlags.N;
   flags->size = tempFlags.size;   
   
   strcpy(flags->path, tempFlags.path);
   
   return OK;
}

void removeDuplicateBar(char * path){ 
   
   if (path[0] == '\0') 
      return; 

   if (path[0] != '.' && path[0] != '/' ) 
      return;

   for (int i = 1; path[i] != '\0'; i++){
      if (path[i] != '/') 
         return;
   }
   
   // If path if of the type ./
   if (path[0] == '.'){
      memset(path, 0, MAX_PATH);
      strcpy(path, ".");
   }
   else{
      memset(path, 0, MAX_PATH);
      strcpy(path, "/");
   }
} 

int validatePath(char * path){

   struct stat stat_buf;

   // If the path is empty, consider the current directory
   if (strcmp(path, "") == OK){
      memset(path, 0, MAX_PATH);
      strcpy(path, ".");
      return OK;
   }
   
   removeDuplicateBar(path);

   if (lstat(path, &stat_buf) == OK) 
      return OK;
   
   return ERROR;
}

int getStatus(int flag_L, struct stat * stat_buf, char * path){
   
   if(!flag_L){ // Using lstat, if -L not specified - showing info about the link itself
      if (lstat(path, stat_buf)) 
         return ERRORARGS;
   }
   else{ // Using stat to follow symbolic links - dereferencing the link
      if (stat(path, stat_buf))
         return ERRORARGS;
   }

   return OK;
}

int currentDirSize(int flags_b, struct stat * stat_buf){
   if (flags_b){
      return stat_buf->st_size;
   }
   else { 
      return stat_buf->st_blocks * 512;
   }
}

long int searchSubdirs(DIR * dirp, flagMask * flags, int stdout_fd){

   struct dirent *direntp;
   struct stat stat_buf;
   long int totalSize=0;

   while ((direntp = readdir(dirp)) != NULL) {

      char pathname[MAX_PATH];
      
      if (pathname == NULL) 
         error_sys("Memory Allocation error\n");

      if(strcmp("/",flags->path) == 0){
          if (snprintf(pathname,MAX_PATH, "%s%s", flags->path, direntp->d_name) < 0) 
            error_sys("New path allocation error\n");
      }
      else{
          if(snprintf(pathname,MAX_PATH, "%s/%s", flags->path, direntp->d_name) < 0) 
            error_sys("New path allocation error\n");
      }

      if (validPathSize(pathname) != OK ){
         logSEND_SIGNAL(SIGUSR1,getParentPid());
         kill(getParentPid(),SIGUSR1);

         logEXIT(ERRORARGS);
         exit(ERRORARGS);
      }

      if(getStatus(flags->L, &stat_buf, pathname) != OK){
         fprintf(stderr, "Stat error in %s\n", pathname);
         logEXIT(ERRORARGS);
         exit(ERRORARGS);
      }

      int isDot = strcmp(direntp->d_name, ".") != 0;
      int isDoubleDot = strcmp(direntp->d_name, "..") != 0;

      // Starts processing the subdirectory
      if (S_ISDIR(stat_buf.st_mode) && isDot && isDoubleDot){

         totalSize += processSubdir(stdout_fd, flags, pathname);

         // Each iteration represents an element (file or subdirectory) of the current directory
         // The --max-depth current value must not be changed by previous subdirectories that were found
         if (flags->d)
            flags->N++;
      }
   }

   return totalSize;
}

long int processSubdir(int stdout_fd, flagMask * flags, char * subDirPath){

   int fd1[2], fd2[2];
   long int subDirSize;
   pid_t pid; 

   // Creating the needed pipes
   if (pipe(fd1) < 0 || pipe(fd2) < 0)
      error_sys("Pipe error!\n");

   // Entering a subdirectory, the --max-depth value is decremented
   if (flags->d)
      flags->N--;

   // Creating the child process
   if ((pid = fork()) < 0) 
      error_sys("Fork error!\n");
      
   if (pid > 0){ //PARENT

      gid = pid;

      close(fd1[READ]);    // pipe1 (parent -> child) - parent process writes the flags on the pipe1 (do not read)
      close(fd2[WRITE]);   // pipe2 (child -> parent) - parent process reads the child subdirectory size (do not write)

      // Saving the current path for later use and replacing it by the path of the subdirectory
      char tempPath[MAX_PATH];
      strcpy(tempPath,flags->path);
      memset(flags->path,'\0',MAX_PATH);
      strcpy(flags->path, subDirPath);

      // Writing flags to pipe1
      write(fd1[WRITE],flags,sizeof(flagMask));
      close(fd1[WRITE]);

      char msg[sizeof(flagMask)];
      flagsToString(flags,msg);
      logSEND_PIPE(msg);

      // Recovering original path
      memset(flags->path,'\0',MAX_PATH);
      strcpy(flags->path,tempPath);

      int status;

      while(true){
         if (waitpid(pid, &status, 0) == -1) 
            break;
      }

      // Reading subdirectory size from pipe2
      read(fd2[READ],&subDirSize,sizeof(long int)); 
      close(fd2[READ]);

      char msg2[17];
      sprintf(msg2, "subDirSize = %ld", subDirSize);
      logRECV_PIPE(msg2);
   }
   else{ //CHILD
      if(getppid() == getParentPid()){
         if(setpgid(0, 0) != 0)
            error_sys("setpgid() error\n");
      }

      close(fd1[WRITE]);   // pipe1 (parent -> child) - child process reads the flags from pipe1 (do not write)
      close(fd2[READ]);    // pipe2 (child -> parent) - child process writes the subdirectory size to pipe2 (do not write)

      dup2(fd1[READ],STDIN_FILENO); // Performing dup for later reading from pipe1

      dup2(fd2[WRITE],STDOUT_FILENO); // Performing dup for later writing to pipe2

      logSEND_SIGNAL(SIGUSR2,getpid());
      kill(getpid(),SIGUSR2);
   
      char stdoutStr[10];

      // Converting old stdout descriptor to string to be passed as an argument
      sprintf(stdoutStr , "%d", stdout_fd);
      
      // Finally, executing it all again; Now for another subdirectory
      execl("simpledu", "simpledu", stdoutStr, NULL);

      error_sys("Exec error!\n");
   } 

   // The total subdirectory size to be passed to the parent directory
   if (!flags->S)
      return subDirSize;

   else
      return 0;   
}

long int searchFiles(DIR * dirp, flagMask * flags, int stdout_fd){

   struct dirent *direntp;
   struct stat stat_buf;
   long int size = 0;

   // Searching for regular files and symbolic links in the current directory
   while ((direntp = readdir(dirp)) != NULL) {
      
      char pathname[MAX_PATH];

      if (pathname == NULL) 
         error_sys("Memory Allocation error\n");
   
      // Saves the path of the file found
      if(strcmp("/",flags->path) == 0){
          if (snprintf(pathname,MAX_PATH, "%s%s", flags->path, direntp->d_name) < 0) 
            error_sys("New path allocation error\n");
      }
      else{
          if(snprintf(pathname,MAX_PATH, "%s/%s", flags->path, direntp->d_name) < 0) 
            error_sys("New path allocation error\n");
      }

      if (validPathSize(pathname) != OK ){
         logSEND_SIGNAL(SIGUSR1,getParentPid());
         kill(getParentPid(),SIGUSR1);

         logEXIT(ERRORARGS);
         exit(ERRORARGS);
      }

      if(getStatus(flags->L,&stat_buf,pathname)){
         fprintf(stderr, "Stat error in %s\n", pathname);
         logEXIT(ERROR);
         exit(ERROR);
      }
   
      if (S_ISREG(stat_buf.st_mode) || S_ISLNK(stat_buf.st_mode)){
         size += dirFileSize(flags,&stat_buf,pathname,stdout_fd);

         // The --max-depth value is incremented to return to the previous sub-level
         if (flags->d)
            flags->N++;
      } 
   }

   return size;
}

long int dirFileSize(flagMask * flags, struct stat * stat_buf, char * pathname, int stdout_fd){
   
   long int tempSize = 0, size = 0;

   // When in the presence of a file, --max-depth value is also decremented
   if (flags->d)
      flags->N--;

   // Regular file (may inclusively be the reference of a symbolic link if -L is active)
   if (S_ISREG(stat_buf->st_mode)){

      // Calculating its size according to the flags
      
      // ./simpledu -b || ./simpledu -B <size> -b || ./simpledu -b -B 1 -> all these situations are equal to -b
      if (flags->b && !flags->B){
         size = stat_buf->st_size;
      }

      // ./simpledu -B <size> (size != 1)
      else if (flags->B && !flags->b){
         size  = stat_buf->st_blocks * 512;
         tempSize = size;
         size  = sizeInBlocks(size,flags->size);
      }

      // ./simpledu -b -B <size>  (size != 1)
      else if (flags->B && flags->b){
         size  = stat_buf->st_size;
         tempSize = size;
         size  = sizeInBlocks(size,flags->size);
      }

      // ./simpledu (without options => default)
      else{
         size  = stat_buf->st_blocks * 512;
         tempSize = size;
         size  = sizeInBlocks(size,1024);
      }
   }

   // Symbolic link (-L NOT active)
   else if (S_ISLNK(stat_buf->st_mode)){

      // Calculating its size according to the flags

      // ./simpledu -b || ./simpledu -B <size> -b || ./simpledu -b -B 1 -> all these situations are equal to -b
      // Size of the Symbolic Link itself in bytes (without dereferencing)
      if(flags->b && !flags->B){
         size = stat_buf->st_size;
      }

      // Size of the Symbolic Link itself in blocks (without dereferencing)
      else{ 

         // ./simpledu -b -B <size>  (size != 1)
         if(flags->B && flags->b){
            size  = stat_buf->st_size;
            tempSize = size;
            size  = sizeInBlocks(size,flags->size);
         }
         else 
            size = 0;
      }
   }

   printFileInfo(flags,size,pathname,stdout_fd);

   logENTRY(size,pathname);

   // For -B option, we want to show one size, but pass another to the total size calculation
   if(flags->B || (!flags->B && !flags->b)) 
      size = tempSize;

   
   return size;
}

long int sizeInBlocks(long int bytesSize, long int blockSize){
   if(bytesSize % blockSize == 0)
      return bytesSize  / blockSize;
   else
      return bytesSize  / blockSize + 1;
}

long int regularFileSize(flagMask * flags, struct stat * stat_buf){
   
   long int totalSize=0;

   // Calculating its size according to the flags:
   // -b || -B 1 -b || -b -B 1 -> all these situations are equal to -b

   if (flags->b && !flags->B){
      return stat_buf->st_size;
   }
   else if (flags->B && !flags->b){
      totalSize = stat_buf->st_blocks * 512;
      return sizeInBlocks(totalSize,flags->size);
   }
   else if (flags->B && flags->b){ // size_b > 1
      return sizeInBlocks(stat_buf->st_size,flags->size);
   }
   else{ // simpledu without options = default
      totalSize = stat_buf->st_blocks * 512;
      return sizeInBlocks(totalSize,1024);
   }
}

long int symbolicLinkSize(flagMask * flags, struct stat * stat_buf){

   long int totalSize=0;
   
   // Calculating its size according to the flags:
   // -b || -B 1 -b || -b -B 1 -> all these situations are equal to -b

   if (flags->b && !flags->B){
      return stat_buf->st_size; // Counting the size of the link itself in bytes
   }
   else{ 
      if(!flags->L){
         if(flags->B && flags->b){
            return sizeInBlocks(stat_buf->st_size,flags->size);
         }
         return 0;
      }
      else{ // Dereferencing symbolic links
         if (flags->B && !flags->b){
            totalSize = stat_buf->st_blocks * 512;
            return sizeInBlocks(totalSize,flags->size);
         }
         else if(flags->B && flags->b){
            return sizeInBlocks(stat_buf->st_size, flags->size);
         }
         else{ // simpledu without options = default
            totalSize = stat_buf->st_blocks * 512;
            return sizeInBlocks(totalSize,1024);
         }
      }
   }
}

void printFileInfo(flagMask * flags, long int size, char * pathname, int stdout_fd){
   // Printing all regular files if --all (-a) is active

   // If --max-depth is active, depth >= 0 and --all is active
   if(flags->a && flags->d && (flags->N >= 0))
      dprintf(stdout_fd,"%-ld\t%-s\n", size, pathname);

   // If --max-depth is NOT active and --all is active
   else if (!flags->d && flags->a)
      dprintf(stdout_fd,"%-ld\t%-s\n", size, pathname);
}

void printDirInfo(flagMask * flags, long int size, int stdout_fd){

   // If --max-depth is active, depth >= 0
   if (flags->d && flags->N >= 0)
      dprintf(stdout_fd,"%-ld\t%s\n", size, flags->path);
   
   // If --max-depth is NOT active
   else if (!flags->d)
      dprintf(stdout_fd,"%-ld\t%s\n", size, flags->path);
}