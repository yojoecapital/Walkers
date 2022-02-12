//User program to compare walkers
#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "walkerData.h"

void runBothWalkers(int dir_in, int ino_in){
   if(fork()==0){
      //redirect standard output
      close(1);
      dup(dir_in);

      char *child_argv[] = {"directoryWalker","/",(char*)0};
      exec(child_argv[0],child_argv);
   }

   if(fork()==0){
      //redirect standard output
      close(1);
      dup(ino_in);

      char *child_argv[] = {"inodeTBWalker",(char*)0};
      exec(child_argv[0],child_argv);
   }
}

void gen_filename(char *buf, uint n){
	char *prefix = "/lost+found-";
	int i =0; //our position within the buff
	while (prefix[i] != '\0'){
		buf[i] = prefix[i];
		i++;

	}
	if (n>=1000){
		return;
	}
	else if(n>=100){
		buf[i++] = (n/100) +'0'; //hundreds place
		buf[i++] = ((n/10) % 10) + '0'; //tenth place
		buf[i++] = (n %10) + '0';      //ones place
	}
	else if(n>=10){
		buf[i++] = (n/10) + '0'; //tens place
		buf[i++] = (n %10) + '0'; //ones place
	}
	else{

		buf[i++] = n + '0';

	}
	buf[i] = '\0';


}
int main(void){

   int dir_pipes[2], ino_pipes[2];
   pipe(dir_pipes);
   pipe(ino_pipes);

   runBothWalkers(dir_pipes[1], ino_pipes[1]);
   int dir_out = dir_pipes[0], ino_out = ino_pipes[0];
   //dir_out and ino_out are file descriptors for the walkers' output
   
   
   struct entry wdi;
   struct entry wdd;
   


   int unreachable=0;
     for(int i = 1; i < NINODES; i++){
	   read(ino_out, &wdi, sizeof(wdi));
	   read(dir_out, &wdd, sizeof(wdd));

        if(wdi.type > 0 && wdd.type == 0){
         walkerData[i] = wdi;
         unreachable++;
      }
   }
   
   int hanging[unreachable]; //hanging_inodes will hold the ino for the hanging inodes
   int treeMap[unreachable * unreachable]; //will map the hanging inodes as a 2d array
   memset( treeMap, 0, unreachable*unreachable*sizeof(int) );
   int h = 0;
   
   printf(1, "Number of unreachable files: %d\n", unreachable);

   char name[64];
   for(int i = 1; i < NINODES; i++){ //walkerData will hold the broken inodes (the difference between the walkers)
      if(walkerData[i].type > 0){
         printf(1, "%d %d %d\n", walkerData[i].type, i, walkerData[i].size);
            hanging[h] = i;
            h++;
      }	
   }
   
   for(int i = 0; i < unreachable; i++){
     if(scandir(hanging[i], &treeMap[i * unreachable], unreachable) < 0){
        printf(2, "Walkers: failed to map hanging tree\n");
        wait();
        wait();
        exit();
     }
   }
   
   //takes the treeMap and combines entries that have a rooted connection - arrays with entries beginning with 0 should be ignored
   int target;
   for(int x = 0; x < unreachable; x++)
   {
      target = treeMap[x*unreachable];
      //printf(1, "Considerign %d\n", target);
      for(int y = 0; y < unreachable; y++)
      {
         if(treeMap[y*unreachable] == target || treeMap[y*unreachable] == 0)
            continue;
         for(int z = 1; z < unreachable; z++)
         {
            //printf(1, "is [%d][%d]%d == %d?\n", y, z, treeMap[y*unreachable + z], target);
            if(treeMap[y*unreachable + z] == target){
               int w = z;
               while(treeMap[y*unreachable + w] != 0) w++;
               for(int i = 1; i < unreachable && treeMap[x*unreachable + i] != 0; i++, w++){
                  //printf(1, "[%d][%d] to [%d][%d]\n", x, i, y, w);
                  treeMap[y*unreachable + w] = treeMap[x*unreachable + i];
               }
               treeMap[x*unreachable] = 0;
               //printf(1, "breaking form y loop\n");
               y = unreachable;
               break;
             }
         }
      }
   }

   
   //takes the treeMap and places the nodes that root to the other lost nodes in as lost+found 
   for(int i = 0; i < unreachable; i++)
      if(treeMap[i*unreachable] != 0){
         gen_filename(name, treeMap[i*unreachable]);
         recover(treeMap[i*unreachable], name);
      }

   wait();
   wait();


   exit();
}

