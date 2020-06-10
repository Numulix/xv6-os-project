#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

#define MODELENGTH 10

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

char modestring[MODELENGTH];

char*
get_mode_string(uint mode, uint type)
{
	if (type == T_DIR) modestring[0] = 'd';
	else modestring[0] = '-';

	modestring[1] = (mode & U_R) ? 'r' : '-';
	modestring[2] = (mode & U_W) ? 'w' : '-';
	modestring[3] = (mode & U_X) ? 'x' : '-';

	modestring[4] = (mode & G_R) ? 'r' : '-';
	modestring[5] = (mode & G_W) ? 'w' : '-';
	modestring[6] = (mode & G_X) ? 'x' : '-';

	modestring[7] = (mode & O_R) ? 'r' : '-';
	modestring[8] = (mode & O_W) ? 'w' : '-';
	modestring[9] = (mode & O_X) ? 'x' : '-';

	return modestring;
} 

void
print_info(struct stat* stat)
{
	char temp_name[30];
	char temp_group[30];
	searchuserbyuid(stat->uid, temp_name);
	searchgroupbygid(stat->gid, temp_group);
	printf("%s %s %s ", get_mode_string(stat->mode, stat->type), temp_name, temp_group);
}

void
ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		print_info(&st);
		printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			print_info(&st);
			printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
		}
		break;
	}
	close(fd);
}

int
main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		ls(".");
		exit();
	}
	for(i=1; i<argc; i++)
		ls(argv[i]);
	exit();
}
