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

int total = 0;

void
du(char *path)
{

	char buf[512], *p;
	int fd;

	struct dirent de;
	struct stat st;

	if((fd = open(path,4)) < 0){
		fprintf(2, "du: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "du: cannot stat %s\n", path);
		close(fd);
		return;
	}

    //st.blocks = st.size == 0 ? 0 : st.size/512 + 1;
    printf("%s %d\n", fmtname(path), st.blocks);
    total += st.blocks;

    if(st.type != T_DIR){
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || strcmp(de.name,".") == 0 || strcmp(de.name,"..") == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf("du: cannot stat %s\n", buf);
            return;
        }
        du(buf);
    }

}

int main(int argc, char *argv[])
{
    int i;

    if(argc == 1)
        du(".");

    for(i=1; i<argc; i++)
		du(argv[i]);

    printf("total %d\n",total);
    exit();
}
