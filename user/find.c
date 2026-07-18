#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char *fargv[MAXARG];

char *fname(char *path)
{
    char *p;
	// Find first character after last slash.
	for (p = path + strlen(path); p >= path && *p != '/'; p--)
		;
	return ++p;
}

void find(char *path, char *name, char *fargv[], int i)
{
	int fd;
	struct stat st;

	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	char buf[512];
	char *p;
	struct dirent de;

	switch (st.type) {
	case T_DEVICE:
	case T_FILE:
		if (!strcmp(fname(path), name)) {
			if (fargv == 0) {
				printf("%s\n", path);
			} else {
				fargv[i] = path;
				if (fork() == 0)
					exec(fargv[0], fargv);
				else
					wait(0);
			}
		}
		break;

	case T_DIR:
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';

		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
				continue;

			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;

			if (stat(buf, &st) < 0) {
				printf("find: cannot stat %s\n", buf);
				continue;
			}

			find(buf, name, fargv, i);
		}
    	break;
	}
	close(fd);
}

int
main(int argc, char *argv[])
{
	if(argc < 2)
		exit(0);
 
	if (argc > 3) {
		int i;
		if (!strcmp(argv[3],"-exec")) {
			for (i = 0; i < argc - 4; i++)
				fargv[i] = argv[i + 4];
			find(argv[1], argv[2], fargv, i);
		}
	} else {
		find(argv[1], argv[2], 0, 0);
	}
	exit(0);
}