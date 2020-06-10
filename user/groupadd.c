#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

#define MAXLENGTH 30

static char *group_name;
static char *gid;

static int defautlgid = 1000;

int 
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Invalid number of arguments.\n");
        exit();
    }

    if (argc > 4) {
        printf("Too many arguments.\n");
        exit();
    }

    int i;
    for (i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-g")) {
            if (i+1 >= argc-1) {
                printf("Invalid operation.\n");
                exit();
            }
            gid = argv[i+1];
            i += 1;
        }

        if (i = argc - 1) {
            group_name = argv[i];
        }
    }

    groupadd();

    exit();
}

void
groupadd()
{
    char temp_group_name[MAXLENGTH];
    char temp_gid[5];

    if (group_name == 0) {
        printf("Group name not entered.");
        return;
    }

    char temp_buf[10];
    if (gid == 0) {
        itoa(defautlgid, temp_buf);
        while (checkgid(temp_buf)) {
            defautlgid++;
            itoa(defautlgid, temp_buf);
        }
        strcpy(temp_gid, temp_buf);
    }
    else {
        if (checkgid(gid)) {
            printf("Group with the given GID already exists!\n");
            exit();
        } else
            strcpy(temp_gid, gid);
    }
    strcpy(temp_group_name, group_name);

    int fd;
    if ((fd = open("/etc/group", O_RDWR)) < 0) {
        printf("Error reading file.\n");
        return;
    }

    char group_line[MAXLENGTH*2];

    strcpy(group_line, temp_group_name);
    strcpy(group_line+strlen(group_line), ":");
    strcpy(group_line+strlen(group_line), temp_gid);
    strcpy(group_line+strlen(group_line), ":");
    strcpy(group_line+strlen(group_line), "\n");

    printf("%s", group_line);

    write_in_file(fd, group_line);

}

void
write_in_file(int fd, char *buf)
{
    int w;
    char readbuf[1024];

    read(fd, readbuf, sizeof(readbuf));
    if ((w = write(fd, buf, strlen(buf))) > 0) {
        printf("New group added!\n");
    }
}