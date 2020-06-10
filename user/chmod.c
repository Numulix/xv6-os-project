#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

#define AFLAG 1
#define UFLAG 2
#define GFLAG 3
#define OFLAG 4

#define ADDPERM 1
#define REMOVEPERM 0

#define READ 1
#define WRITE 2
#define EXEC 3

static int flag;
static int modeflag;
static int rwxflag;

int
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Not enough arguments.\n");
        exit();
    }

    int i;
    for (i = 1; i < argc; i++) {

        if (i == 1) {
            char *p = argv[i];
            int j;
            for (j = 0; j < 3; j++) {
                if (*p == '\0') {
                    printf("Invalid operation.\n");
                    exit();
                }

                if (j == 0) {
                    if (*p == 'a') {
                        flag = AFLAG;
                    } else if (*p == 'u') {
                        flag = UFLAG;
                    } else if (*p == 'g') {
                        flag = GFLAG;
                    } else if (*p == 'o') {
                        flag = OFLAG;
                    } else {
                        printf("Invalid operation.\n");
                        exit();
                    }
                } else if (j == 1) {
                    if (*p == '+') {
                        modeflag = ADDPERM;
                    } else if (*p == '-') {
                        modeflag = REMOVEPERM;
                    } else {
                        printf("Invalid operation.\n");
                        exit();
                    }
                } else if (j == 2) {
                    if (*p == 'r') {
                        rwxflag = READ;
                    } else if (*p == 'w') {
                        rwxflag = WRITE;
                    } else if (*p == 'x') {
                        rwxflag = EXEC;
                    } else {
                        printf("Invalid operation.\n");
                        exit();
                    }
                }
                p++;
            }
            continue;
        }

        changemodes(argv[i]);

    }

    exit();
}

void
changemodes(char *path)     
{
    struct stat st;
    int fd;

    if ((fd = open(path, O_RDONLY)) < 0) {
        printf("Cannot open file\n");
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf("Cannot stat\n");
        close(fd);
        return;
    }

    int tomode = 0000;

    if (flag == AFLAG) {
        if (rwxflag == READ)
            tomode |= 0444;
        else if (rwxflag == WRITE)
            tomode |= 0222;
        else if (rwxflag == EXEC)
            tomode |= 0111;
    } else if (flag == UFLAG) {
        if (rwxflag == READ)
            tomode |= 0400;
        else if (rwxflag == WRITE)
            tomode |= 0200;
        else if (rwxflag == EXEC)
            tomode |= 0100;
    } else if (flag == GFLAG) {
        if (rwxflag == READ)
            tomode |= 0040;
        else if (rwxflag == WRITE)
            tomode |= 0020;
        else if (rwxflag == EXEC)
            tomode |= 0010;
    } else if (flag == OFLAG) {
        if (rwxflag == READ)
            tomode |= 0004;
        else if (rwxflag == WRITE)
            tomode |= 0002;
        else if (rwxflag == EXEC)
            tomode |= 0001;
    }

    if (!modeflag) {
        tomode = st.mode & ~(tomode);
    } else {
        tomode = st.mode | tomode;
    }
    
    chmod(path, tomode);
}
