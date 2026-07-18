#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[1024];
char num_buf[1024];

void
print_num(int num_len)
{
	int num;
	num_buf[num_len] = '\0';
	num = atoi(num_buf);
	if (num % 5 == 0 || num % 6 == 0)
		printf("%d\n", num);
}

int
main(int argc, char *argv[])
{
	int fd;
	int k;

	for (k = 1; k < argc; k++) {
		if ((fd = open(argv[k], O_RDONLY)) < 0) {
			printf("sixfive: cannot open %s\n", argv[k]);
			exit(1);
		}

		int i;
		int n;
		int num_len = 0;
		int is_num = 1;

		while ((n = read(fd, buf, sizeof(buf))) > 0) {
			for (i = 0; i < n; i++) {
				if (strchr(" -\r\t\n./,", buf[i])) {
					if(is_num == 1)
						print_num(num_len);
					num_len = 0;
					is_num = 1;
				} else if (buf[i] >= '0' && buf[i] <= '9') {
					num_buf[num_len++] = buf[i];
				} else {
					is_num = 0;
				}
			}
		}

		if (is_num == 1)
			print_num(num_len);
		close(fd);
	}
	exit(0);
}