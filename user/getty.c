#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

#define LENGHT 25

int
main(void) {

    clrscr();

    int pid, wpid, fd;
    int logged = 1;
    char namebuf[LENGHT];
    char passbuf[LENGHT];
    char buf[512];
    int issuefd = open("/etc/issue", O_RDONLY);
    int motdfd = open("/etc/motd", O_RDONLY);
    
    read(issuefd, buf, sizeof(buf));
    printf("%s\n", buf);

    while(logged) {
        printf("Username: ");
        char *username = gets(namebuf, LENGHT);
        printf("Password: ");
        echotr();
        char *password = gets(passbuf, LENGHT);
        echotr();

        dup(0);
        dup(0);

        if((fd = open("/etc/passwd", O_RDONLY)) < 0) {
            printf("getty: cannot open\n");
            exit();
        }

        if (passwordcheck(fd, username, password)) {

            logged = 0;
            close(fd);
            printf("Welcome back %s\n", username);
            read(motdfd, buf, sizeof(buf));
            printf("%s\n", buf);

            pid = fork();
            if (pid < 0) {
                printf("getty: fork failed\n");
                exit();
            }

            if (pid == 0) {
                char *uname[] = {username, 0};
                exec("/bin/sh", uname);
                printf("getty: exec sh failed\n");
                exit();
            }

        } else {
            close(fd);
            printf("wrong username or password\n");
        }

        while((wpid=wait()) >= 0 && wpid != pid)
	        printf("zombie!\n");
        
    }

    wait();
    exit();

}