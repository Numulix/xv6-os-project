#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

#define MAXLENGTH 30

static char *login;
static char *name;
static char *uid;
static char *dir;

static int dirflag = 0;
static int uidflag = 0;
static int nameflag = 0;
static int intuid = 0;
static int defaultuid = 1000;

int 
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Invalid number of arguments.\n");
        exit();
    }

    if (argc > 8) {
        printf("Too many arguments.\n");
        exit();
    }

    int i;
    for (i = 1; i < argc; i++) {
        
        if (!strcmp(argv[i], "-d")) {
            dir = argv[i+1];
            i += 1;
            continue;
        } else if (!strcmp(argv[i], "-c")) {
            name = argv[i+1];
            i += 1;
            continue;
        } else if (!strcmp(argv[i], "-u")) {
            uid = argv[i+1];
            intuid = atoi(argv[i+1]);
            i += 1;
            continue;
        }

        if (i == argc - 1) {
            login = argv[i];
        }

    }
    useradd();

    exit();
}

void
useradd()
{
    char temp_login[MAXLENGTH];
    char temp_name[MAXLENGTH];
    char temp_dir[MAXLENGTH];
    char temp_uid[5];

    if (login == 0) {
        printf("Username wasn't specified\n");
        return;
    }

    if (intuid != 0 && checkuid(uid)) {
        printf("User with this UID already exists.\n");
        return;
    }

    strcpy(temp_login, login);

    if (existinguser(temp_login)) {
        printf("User already exists\n");
        return;
    }
    
    char tempbuf[10];
    if (intuid == 0) {
        itoa(defaultuid, tempbuf);
        while (checkuid(tempbuf)) {
            defaultuid++;
            itoa(defaultuid, tempbuf);
            printf("%s\n", tempbuf);
        }
        strcpy(temp_uid, tempbuf);
    } else {
        strcpy(temp_uid, uid);
    }
    
    if (name == 0) {
        strcpy(temp_name, login);
    } else {
        strcpy(temp_name, name);
    }

    if (dir == 0) {
        strcpy(temp_dir, "/home/");
        strcpy(temp_dir+strlen(temp_dir), login);
    } else {
        strcpy(temp_dir, dir);
    }

    printf("Username: %s\n", temp_login);
    printf("Name: %s\n", temp_name);
    printf("UID: %s\n", temp_uid);
    printf("Home dir: %s\n", temp_dir);

    int fd;
    if ((fd = open("/etc/passwd", O_RDWR)) < 0) {
        printf("Error opening file.\n");
        return;
    }

    char line[MAXLENGTH*4];
    strcpy(line, temp_login);
    strcpy(line+strlen(line), ":default:");
    strcpy(line+strlen(line), temp_uid);
    strcpy(line+strlen(line), ":");
    strcpy(line+strlen(line), temp_uid);
    strcpy(line+strlen(line), ":");
    strcpy(line+strlen(line), temp_name);
    strcpy(line+strlen(line), ":");
    strcpy(line+strlen(line), temp_dir);
    strcpy(line+strlen(line), "\n");

    write_in_file(fd, line);
    close(fd);

    fd = open("/etc/group", O_RDWR);
    char group[MAXLENGTH*2];
    strcpy(group, temp_login);
    strcpy(group+strlen(group), ":");
    strcpy(group+strlen(group), temp_uid);
    strcpy(group+strlen(group), ":");
    strcpy(group+strlen(group), temp_login);
    strcpy(group+strlen(group), "\n");

    write_in_file(fd, group);
    printf("New user added!\n");
    close(fd);

}

void
write_in_file(int fd, char *buf)
{
    int w;
    char readbuf[1024];

    read(fd, readbuf, sizeof(readbuf));
    w = write(fd, buf, strlen(buf));
}
