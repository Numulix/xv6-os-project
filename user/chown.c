#include "kernel/types.h"
#include "user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

int uid = -1;
int gid = -1;

int
main(int argc, char const *argv[])
{
    if (argc < 3) {
        printf("Invalid number of arguments.\n");
        exit();
    }

    for (int i = 1; i < argc; i++) {

        if (i == 1) {
            int l, c;
            l = c = 0;
            char user[30];
            char group[30];

            while (argv[i][c] != ':' && argv[i][c]) {
                user[l++] = argv[i][c++];
            }
            user[l] = '\0';

            if (strcmp(user, "")) {
                if (parseuser(user) < 0) {
                    printf("User does not exist\n");
                    exit();
                }
                uid = parseuser(user);
            }

            if (!argv[i][c]) continue;

            l = 0; c++;

            while (argv[i][c]) {
                group[l++] = argv[i][c++];
            }
            group[l] = '\0';

            if (strcmp(group, "") != 0) {
                if (parsegroup(group) < 0) {
                    printf("Group does not exist\n");
                    exit();
                }
                gid = parsegroup(group);
            }
        }
        changeown(argv[i]);
    }
    
    exit();
}

void
changeown(char *path)
{
    chown(path, uid, gid);
}