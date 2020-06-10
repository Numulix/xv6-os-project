#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

int gid;

int 
main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Invalid number of arguments\n");
        exit();
    }

    int i;
    for (i = 1; i < argc; i++) {
        
        if (i == 1) {
            if (parsegroup(argv[i]) < 0) {
                printf("Group not found\n");
                exit();
            }

            gid = parsegroup(argv[i]);

            continue;
        }

        changegroup(argv[i]);

    }

    exit();
}

void
changegroup(char *path)
{
    if (open(path, O_RDONLY) < 0) {
        printf("Cannot open\n");
        exit();
    }

    chown(path, -1, gid);
}