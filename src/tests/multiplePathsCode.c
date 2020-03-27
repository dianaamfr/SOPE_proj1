/*Se decidirmos implementar para vários paths fica aqui a versão inicial com essa implementação*/

int validatePaths(char** path, char * stringPaths){

   struct stat stat_buf;

   if(strcmp(stringPaths, "") == OK){
      strcpy(path[0], ".");
      return OK;
   }

   int  j = 0, k = 0;
   for(int i = 0; i < strlen(stringPaths); i++){
      if(stringPaths[i] == ' '||stringPaths[i] == '\0'){
         path[k][j] = '\0';
         k++;  
         j = 0;
      }
      else{
         path[k][j] = stringPaths[i];
         j++;
      }
   }
   
   char ** tempPath = (char **) malloc(MAX_NUM_PATHS * sizeof(char *));
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      tempPath[i] = (char *) malloc(sizeof(char) * (MAX_PATH + 1));
      memset(tempPath[i], 0, sizeof(char) * (MAX_PATH + 1));
   }

   j = 0;
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      if (lstat(path[i], &stat_buf) == OK){
         strcpy(tempPath[j], path[i]);
         j++;
      }
   }

   if(strcmp(tempPath[0], "") == OK) 
      return ERRORARGS;

   for(int i = 0; i < MAX_NUM_PATHS; i++)
      memset(path, 0, sizeof(char) * MAX_PATH);

   memcpy(path, tempPath, MAX_NUM_PATHS * MAX_PATH * sizeof(char));
      
   free(tempPath);
   
   return OK;   
}

int main(int argc, char* argv[], char* envp[]){

   flagMask flags;
   DIR *dirp;
   struct dirent *direntp;
   struct stat stat_buf;
   long totalSize = 0;

   char ** paths = (char**)malloc(MAX_NUM_PATHS*sizeof(char*));;
   for(int i = 0; i < MAX_NUM_PATHS; i++){
      paths[i] = (char*)malloc(sizeof(char)*(MAX_PATH+1));
      memset(paths[i], 0, sizeof(char)*(MAX_PATH+1));
   }

   if(checkArgs(argc,argv,&flags) != OK){
      printf("Usage: %s -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]\n",argv[0]);
      exit(ERRORARGS);
   }

   if (flags.d < 0 || flags.size < 0) {
      exit(ERRORARGS);
   }

   printf("###########################\n");

   printf("RUNNING...\n");

   printf("a: %d\n", flags.a);

   printf("b: %d\n", flags.b);

   printf("B: %d\n", flags.B);

   printf("l: %d\n", flags.l);

   printf("L: %d\n", flags.L);

   printf("S: %d\n", flags.S);

   printf("max-depth: %d", flags.d);
   if(flags.d)
      printf(" value=%d", flags.N);
   printf("\n");

   printf("size: %ld\n",flags.size);

   printf("path: %s\n", flags.path);

   printf("###########################\n");


   if(validatePaths(paths, flags.path) != OK) 
      exit(ERRORARGS);


   for(int i = 0; i < MAX_NUM_PATHS && strcmp(paths[i],"") != OK; i++){
      strcpy(flags.path,paths[i]);

      if (flags.a == 1) {

         if (lstat(flags.path, &stat_buf)){ 
            fprintf(stderr, "Stat error in %s\n", flags.path);
            return 1;
         }

         if (S_ISDIR(stat_buf.st_mode)) {


            totalSize += ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);

            if ((dirp = opendir(flags.path)) == NULL)
               fprintf(stderr, "Could not open directory %s\n", flags.path);

            while ((direntp = readdir(dirp)) != NULL) {

               if (lstat(direntp->d_name, &stat_buf)){ 
                  fprintf(stderr, "Stat error in %s\n", direntp->d_name);
                  return 1;
               }

               if (S_ISREG(stat_buf.st_mode)) {

                  char *pathname; //para guardar o path de cada ficheiro ou subdiretório

                  pathname = malloc(strlen(flags.path) + 1 + strlen(direntp->d_name) + 1);

                  if (pathname == NULL) {
                     fprintf(stderr, "Memory Allocation error\n");
                     exit(1);
                  }
               
                  //guarda o path do ficheiro ou subdiretorio
                  sprintf(pathname, "%s/%s", flags.path, direntp->d_name);

                  long int temp = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
                  printf("%-10ld  %-10s\n", temp, pathname);
                  totalSize += ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);

                  free(pathname);

               }
            }

            printf("%ld\n", totalSize);
            closedir(dirp);
         }

         else if (S_ISREG(stat_buf.st_mode)) {

            printf("hello\n");

            if (flags.b) {
               totalSize = stat_buf.st_size;
            }

            else if (flags.B) {
               totalSize = stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize);
               totalSize = ceil((double)totalSize / flags.size);
            }

            else {
               totalSize = (int)ceil(stat_buf.st_blksize*ceil((double)stat_buf.st_size/stat_buf.st_blksize)/1024);
            }

            printf("%-10ld  %-10s\n", totalSize, flags.path);
         }
      }
   }

   free(paths);

   exit(OK);
}