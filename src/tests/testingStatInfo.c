#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>

#define FLOAT_TO_INT(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define MAX_PATH_SIZE 300
int main(int argc, char *argv[])
{
 DIR *dirp;
 struct dirent *direntp;
 struct stat stat_buf;
 char name[MAX_PATH_SIZE];
 int nBytes = 0;

 if (argc != 2)
 {
  fprintf( stderr, "Usage: %s dir_name\n", argv[0]);
  exit(1);
 }

 if ((dirp = opendir( argv[1])) == NULL)
 {
  perror(argv[1]);
  exit(2);
 }


while ((direntp = readdir( dirp)) != NULL)
 {
   sprintf(name,"%s/%s",argv[1],direntp->d_name);// <----- NOTAR //guarda o path+nome
  if (lstat(name, &stat_buf)==-1)   // testar com stat()
  {
   perror("lstat ERROR");
   exit(3);
  }

  //du -l -a -b
  /*if(strcmp(direntp->d_name,"..") != 0){
     if (strcmp(direntp->d_name,".") == 0) {
        nBytes += stat_buf.st_size;
        continue;
     }
     printf("%ld  %-25s\n", stat_buf.st_size, direntp->d_name);
     nBytes += stat_buf.st_size;
  }*/

  //du -l -a
  if(strcmp(direntp->d_name,"..") != 0){
     if (strcmp(direntp->d_name,".") == 0) {
        nBytes += stat_buf.st_size;
        continue;
     }
     printf("%ld  %-25s\n", (double)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024), direntp->d_name);
     nBytes += ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
   }
 }
 //printf("%d  .\n", nBytes);

 closedir(dirp);
 exit(0);
}

