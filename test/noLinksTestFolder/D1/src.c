#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#define MAX_NAME_SIZE   50
#define MAX_GRADE_SIZE  2
#define MAX_STUDENTS    20

struct student{
   int grade;
   char name[MAX_NAME_SIZE+1];
};

int main(int argc, char *argv[]){
   int fd;
   struct student s;

   if(argc != 2){
      printf("Usage: %s <source>\n",argv[0]);
      exit(1);
   }

   if((fd = open(argv[1],O_RDONLY)) == -1 ){
      perror(argv[1]);
      exit(2);
   }

   while(read(fd,&s,sizeof(struct student)) != 0){

      printf("Name: %s  Grade: %d\n",s.name,s.grade);

   }

   close(fd);

   return 0;
}
