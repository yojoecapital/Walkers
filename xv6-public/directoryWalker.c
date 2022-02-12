#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "walkerData.h"

int human_readable_mode =0;
	void
ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		printf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){

		case T_DEV:
		case T_FILE:
			if(human_readable_mode){
				printf(1, "%s %d %d %d\n", path, st.type, st.ino, st.size);
			}
			else{
				walkerData[st.ino].type = st.type;
				walkerData[st.ino].size = st.size;
			}
			break;

		case T_DIR:
			if(human_readable_mode){
				printf(1, "%s %d %d %d\n", path, st.type, st.ino, st.size);
			}
			else {
				walkerData[st.ino].type = st.type;
				walkerData[st.ino].size = st.size;
			}

			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf(1, "ls: path too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf+strlen(buf);
			*p++ = '/';
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				if(de.inum == 0)
					continue;
				if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0){
					continue;
				}
				memmove(p, de.name, DIRSIZ);
				//end of the string
				p[DIRSIZ] = 0;
				if(stat(buf, &st) <0){
					printf(1, "ls: cannot stat %s\n", buf);
					continue;
				}
				//buf will contain the path
				ls(buf);
			}
			break;

		default:
			printf(1,"??? %s\n", path);
			break;
	}
	close(fd);
}

	int
main(int argc, char *argv[])
{
	int i;


	if(argc >=2 && strcmp(argv[argc-1], "-h") == 0){
		argc--;
		human_readable_mode=1;

	}
	if(argc < 2){
		ls(".");
		if(!human_readable_mode)
		{
			for(int i=1;i<200;i++){
				write(1,&walkerData[i], sizeof(walkerData[i]));
			}
		}
		exit();
	}

	for(i=1; i<argc; i++){
		ls(argv[i]);
	}

	if(!human_readable_mode){
		for(int i=1;i<200;i++){
			write(1,&walkerData[i], sizeof(walkerData[i]));
		}

	}

	exit();
}
