#include "user.h"
#define NINODES 200

struct entry {
   int type;
   int size;
};

struct entry walkerData[NINODES] = {0};

void printWalkerData(int count) //for debugging
{
  if(count > NINODES)
     count = NINODES;
  struct entry wdi[NINODES] = {0};
  struct entry wdd[NINODES] = {0};
  int fdi = open("ino_data", O_RDWR);
  if(fdi < 0)
     return;
  read(fdi, (char *)wdi, sizeof(wdi));
  
  
  close(fdi);
  int fdd = open("dir_data", O_RDWR);
  if(fdd < 0)
     return;
  read(fdd, (char *)wdd, sizeof(wdd));
  close(fdd);
  for(int i = 0; i < count; i++)
     printf(1, "ino %d %d %d \t\t dir %d %d %d \n", wdi[i].type, i, wdi[i].size, wdd[i].type, i, wdd[i].size);
}
