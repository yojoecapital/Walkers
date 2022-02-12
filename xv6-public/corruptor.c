#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int zero(char *path)
{
   //char buf[512], *p;
   int fd;
   struct stat st;

   if((fd = open(path, 0)) < 0){
     printf(2, "corruptor: cannot open %s\n", path);
     return -1;
   }
   
   if(fstat(fd, &st) < 0){
     printf(2, "ls: cannot stat %s\n", path);
     close(fd);
     return -1;
  }
  
  printf(1, "%s %d %d %d\n", path, st.type, st.ino, st.size);
  
  if(zerout(st.ino) < 0){
     printf(2, "corruptor: cannot curropt directory %s\n", path);
     return -1;
  }
  return 0;
}

int main(int argc, char *argv[])
{
   int i;
   for(i = 1; i < argc; i++){
     if(zero(argv[i]) < 0)
        exit();
     unlink(argv[i]);
   }
   exit();
}
