#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"


char*
fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

int symlinkinfo(char *path){
    char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path,4)) < 0){
		fprintf(2, "symlinkinfo: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "symlinkinfo: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){

		case T_DIR:
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("symlinkinfo: path too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf+strlen(buf);
			*p++ = '/';

			int fd2;
			struct stat st2;
			while(read(fd, &de, sizeof(de)) == sizeof(de)){

				if(de.inum == 0)
					continue;
				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;

				fd2 = open(buf,4);
				fstat(fd2,&st2);

				char c[st2.size];

				if(st2.type == T_SYMLINK){

					read(fd2, c, st2.size);
					printf("%s-> %s\n", fmtname(buf), c);
				}
				close(fd2);
			}

			break;

		case T_SYMLINK:
			char ch[st.size];
			read(fd,ch,st.size);
			printf("%s-> %s\n", fmtname(path), ch);
			close(fd);
			break;
	}
	close(fd);
}

int main(int argc, char *argv[]){
    if(argc == 1)
        symlinkinfo(".");
    else if(argc != 2)
        printf("Usage: symlinkinfo ? arg\n");
    else
        symlinkinfo(argv[1]);

    exit();
}
