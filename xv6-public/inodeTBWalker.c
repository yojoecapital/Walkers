#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "walkerData.h"

	int
main(int argc, char *argv[])
{ 
	int human_readable_mode = argc >1;

	for(int i = 1; i <= 200; i++)
	{
		int size;
		int type;
		if((type =  imeta(i, &size)) < 0){
			printf(2, "imeta failed with %d\n", type);
			exit();
		}

		/* #define T_DIR  1   // Directory
		#define T_FILE 2   // File
		#define T_DEV  3   // Device    */
		if (human_readable_mode){
			if(type > 0) //if inode is allocated
			{
				printf(1,"%d %d %d\n", type, i, size);
			}
		}
		else {
			struct entry e= {type,size};
			write(1,&e,sizeof(struct entry));

		}	
	}
	

	exit();
}
