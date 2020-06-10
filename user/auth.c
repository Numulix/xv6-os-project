#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"

int
passwordcheck(int fd, char *user, char *password)
{

	int i, n, c, l;
	l = c = 0;
	char temp_user[30];
	char temp_pass[30];
	char temp_uid[30];
	char temp_euid[30];
	char temp_name[30];
	char temp_homedir[30];
	char buf[1024];

	
	if (user[strlen(user) - 1] == '\n') {
		user[strlen(user) - 1] = '\0';
	}

	if (password[strlen(password) - 1] == '\n') {
		password[strlen(password) - 1] = '\0';
	}
	

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		for (i = 0; i < n; ) {

			if (l == 0) {
				while (i < n && buf[i] != ':') 
					temp_user[c++] = buf[i++];
				if (i == n)
					break;
				temp_user[c] = '\0';
				i++;
			}

			while (i < n && buf[i] != ':')
				temp_pass[l++] = buf[i++];
			if (i == n)
				break;
			temp_pass[l] = '\0';
			i++;

			l = c = 0;
			
			while (i < n && buf[i] != ':')
				temp_uid[l++] = buf[i++];
			if (i == n)
				break;
			temp_uid[l] = '\0';
			i++;

			while (i < n && buf[i] != ':')
				temp_euid[c++] = buf[i++];
			if (i == n)
				break;
			temp_euid[c] = '\0';
			i++;

			l = c = 0;

			while (i < n && buf[i] != ':')
				temp_name[l++] = buf[i++];
			if (i == n)
				break;
			temp_name[l] = '\0';
			i++;

			while (i < n && buf[i] != '\n')
				temp_homedir[c++] = buf[i++];
			if (i == n)
				break;
			temp_homedir[c-1] = '\0';

			if (!strcmp(user, temp_user) && !strcmp(password, temp_pass)) {
				mkdir(temp_homedir);
				setuid(atoi(temp_uid));
				chown(temp_homedir, atoi(temp_uid), atoi(temp_uid));
				setgroups(1, atoi(temp_uid));
				close(fd);
				return 1;
			}

			l = c = 0;

			while (i < n && buf[i++] != '\n');
		}
	}
	close(fd);
	return 0;
}

int
existinguser(char *username)
{
    int i, fd, l, n;
    char iuser[30];
    char buf[1024];

    fd = open("/etc/passwd", O_RDONLY);
    l = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {

        for (i = 0; i < n;) {

            while (i < n && buf[i] != ':')
                iuser[l++] = buf[i++];

            if (i == n) break;

            iuser[l] = '\0';
            l = 0;

            if (!strcmp(username, iuser)) {
                close(fd);
                return 1;
            }

            while (i < n && buf[i++] != '\n');
        }
    }

    close(fd);
    return 0;
}

int
existinggroup(char *group)
{
	int i, fd, l, n;
    char igroup[30];
    char buf[1024];

	fd = open("/etc/group", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		for (i = 0; i < n;) {

			while (i < n && buf[i] != ':')
				igroup[l++] = buf[i++];

			if (i == n) break;
			igroup[l] = '\0';
			l = 0;

			if (!strcmp(group, igroup)) {
				close(fd);
				return 1;
			}

			while (i < n && buf[i++] != '\n');
		}
	}

	close(fd);
	return -1;
}

int
checkuid(char *uid)
{
	int i, fd, l, n, k;
	char iuid[10];
	char buf[1024];

	fd = open("/etc/passwd", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {

			for (k = 1; k <= 2; k++) {
				while (buf[i] != ':' && i < n) i++;
				i++;
			}

			while (buf[i] != ':' && i < n) 
				iuid[l++] = buf[i++];
			iuid[l] = '\0';

			if (i == n) break;

			if (!strcmp(uid, iuid)) {
				close(fd);
				return 1;
			}

			l = 0;
			while (buf[i++] != '\n' && i < n);
		}

	}
	close(fd);
	return 0;
}

int
checkgid(char *gid)
{
	int i, fd, l, n;
	char igid[10];
	char buf[1024];

	fd = open("/etc/group", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {

			
			while (buf[i] != ':' && i < n) i++;
			i++;

			while (buf[i] != ':' && i < n) 
				igid[l++] = buf[i++];
			igid[l] = '\0';

			if (i == n) break;

			if (!strcmp(gid, igid)) {
				close(fd);
				return 1;
			}

			l = 0;
			while (buf[i++] != '\n' && i < n);
		}

	}
	close(fd);
	return 0;
}

void
swap(char *x, char *y)
{
	char t = *x;
	*x = *y;
	*y = t;
}

char*
reverse(char *buf, int i, int j)
{
	while (i < j)
		swap(&buf[i++], &buf[j--]);
}

char*
itoa(int value, char *buf)
{
	int n;
	if (value < 0) n = value*(-1);
	else n = value;

	int i = 0;
	while (n) {
		int r = n % 10;
		if (r >= 10)
			buf[i++] = 65 + (r - 10);
		else
			buf[i++] = 48 + r;

		n = n / 10;
	}

	if (i == 0) buf[i++] = '0';

	if (value < 0) buf[i++] = '-';
	buf[i] = '\0';

	return reverse(buf, 0, i-1);
}

int
searchuserbyuid(int uid, char *name)
{
	char dummy[5];
	itoa(uid, dummy);
	if (!checkuid(dummy)) return 0;

	char uidstr[5];
	char buf[1024];

	char temp_user[30];
	int i, l, fd, n;
	fd = open("/etc/passwd", O_RDONLY);
	l = 0;
	
	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {

			while (i < n && buf[i] != ':') {
				temp_user[l++] = buf[i++];
			}
			temp_user[l] = '\0';
			

			i++;
			if (i == n) break;
			l = 0;

			while (i < n && buf[i] != ':') i++;
			i++;
			if (i == n) break;

			while (i < n && buf[i] != ':') {
				uidstr[l++] = buf[i++];
			}
			uidstr[l] = '\0';

			if (uid == atoi(uidstr)) {
				close(fd);
				strcpy(name, temp_user);
				return 1;
			}
			
			l = 0;
			while (buf[i++] != '\n' && i < n);
		}

	}
	close(fd);
	return -1;
}

int
searchgroupbygid(int gid, char *group)
{
	char temp_gname[30];
	char temp_gid[5];
	char buf[1024];
	int i, l, fd, n;
	fd = open("/etc/group", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {

			while (i < n && buf[i] != ':') {
				temp_gname[l++] = buf[i++];
			}
			temp_gname[l] = '\0';

			i++;
			if (i == n) break;
			l = 0;

			while (i < n && buf[i] != ':') {
				temp_gid[l++] = buf[i++];
			}
			temp_gid[l] = '\0';

			if (gid == atoi(temp_gid)) {
				close(fd);
				strcpy(group, temp_gname);
				return 1;
			}
			l = 0;
			while (i < n && buf[i++] != '\n');
		}

	}
	close(fd);
	return -1;
}

int
getgidbyname(char *group)
{
	int i, fd, n, l;
	char igroup[30];
	char igid[6];
	char buf[1024];

	fd = open("/etc/group", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n; ) {

			while (i < n && buf[i] != ':')
				igroup[l++] = buf[i++];
			igroup[l] = '\0';
			
			i++;
			if (i == n) break;
			l = 0;

			while (i < n && buf[i] != ':')
				igid[l++] = buf[i++];
			igid[l] = '\0';

			if (!strcmp(group, igroup)) {
				close(fd);
				int gid = atoi(igid);
				return gid;
			}

			l = 0;
			while (i < n && buf[i++] != '\n');
		}

	}
	close(fd);
	return -1;
}

int
parseuser(char *str)
{
	if (!existinguser(str) && !checkuid(str)) return -1;

	int i, n, fd, l;
	char iname[30];
	char iuid[6];
	char buf[1024];

	fd = open("/etc/passwd", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {
			
			while (i < n && buf[i] != ':')
				iname[l++] = buf[i++];
			iname[l] = '\0';

			i++;
			if (i == n) break;
			l = 0;

			while (i < n && buf[i] != ':') i++;
			i++;

			while (i < n && buf[i] != ':')
				iuid[l++] = buf[i++];
			iuid[l] = '\0';

			if (!strcmp(str, iname) || !strcmp(str, iuid)) {
				close(fd);
				int uid = atoi(iuid);
				return uid;
			}

			l = 0;
			while (i < n && buf[i++] != '\n');
		}

	}
	close(fd);
	return -1;
}

int
parsegroup(char *str)
{
	if (!existinggroup(str) && !checkgid(str)) return -1;

	int i, n, fd, l;
	char igroup[30];
	char igid[6];
	char buf[1024];

	fd = open("/etc/group", O_RDONLY);
	l = 0;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {

		for (i = 0; i < n;) {
			
			while (i < n && buf[i] != ':')
				igroup[l++] = buf[i++];
			igroup[l] = '\0';

			i++;
			if (i == n) break;
			l = 0;

			while (i < n && buf[i] != ':')
				igid[l++] = buf[i++];
			igid[l] = '\0';

			if (!strcmp(str, igroup) || !strcmp(str, igid)) {
				close(fd);
				int gid = atoi(igid);
				return gid;
			}

			l = 0;
			while (i < n && buf[i++] != '\n');
		}

	}
	close(fd);
	return -1;
}

