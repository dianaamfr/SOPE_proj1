#include "aux.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

int validatePath(char * path){

   struct stat stat_buf;

   //if empty path consider the current directory
   if(strcmp(path, "") == OK){
      memset(path,0,MAX_PATH);
      strcpy(path, ".");
      return OK;
   }

   if(lstat(path, &stat_buf) == OK) 
      return OK;
   
   return ERROR;
}

void printFlags(flagMask * flags, char * description){
   printf("###########################\n");

   printf("%s...\n",description);

   printf("a: %d\n", flags->a);

   printf("b: %d\n", flags->b);

   printf("B: %d\n", flags->B);

   printf("l: %d\n", flags->l);

   printf("L: %d\n", flags->L);

   printf("S: %d\n", flags->S);

   printf("max-depth: %d", flags->d);
   if(flags->d)
      printf(" value=%d", flags->N);
   printf("\n");

   printf("size: %ld\n",flags->size);

   printf("path: %s\n", flags->path);

   printf("###########################\n");
}

long int dirFileSize(flagMask *flags, struct stat *stat_buf, char * pathname, int stdout_fd){
   
   long int sizeBTemp = 0, size = 0;

   if (S_ISREG(stat_buf->st_mode)){//if it is a regular file
      //calculate the space in disk of the regular file according to the active options
      // -b || -B 1 -b || -b -B 1  => -b
      if (flags->b && !flags->B){
         size = stat_buf->st_size;
      }
      else if(flags->B && !flags->b){
         size  = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
         sizeBTemp = size;
         size  = ceil((double)size / flags->size);
      }
      else if(flags->B && flags->b){
         size  = stat_buf->st_size;
         sizeBTemp = size;
         size  = ceil((double)size / flags->size);
      }
      else{ // du without options - default
         size = (int)ceil(stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize)/1024);
      }
   }

   else if(S_ISLNK(stat_buf->st_mode)){
      if(flags->b && !flags->B){// -b || -B 1 -b || -b -B 1  => -b
         size = stat_buf->st_size; //count size of the link itself in bytes
      }
      else{ 
         if(!flags->L){
            if(flags->B && flags->b){
               size  = stat_buf->st_size;
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else size = 0;
         }
         else{ //dereference symbolic links
            if (flags->B && !flags->b){
               size  = stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize);
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else if(flags->B && flags->b){
               size  = stat_buf->st_size;
               sizeBTemp = size;
               size  = ceil((double)size / flags->size);
            }
            else{//du without options - default
               size = (int)ceil(stat_buf->st_blksize*ceil((double)stat_buf->st_size/stat_buf->st_blksize)/1024);
            }
         }
         
      }
   }

   //print all regular files if --all (-a) is active
   if(flags->a)
      dprintf(stdout_fd,"%-8ld  %-10s\n", size, pathname);
   
   //for -B option we want to show one size on screen but pass another to the total size calculation
   if(flags->B) 
      size = sizeBTemp;

   return size;
}
