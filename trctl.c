#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "trace.h"

static int get_bitmap(int fd)
{
	u_int bitmap = 0;

	if (ioctl(fd, TRACE_GET_BITMAP, &bitmap) < 0) {
		perror("ioctl failed");
		return -1;
	}
	printf("bitmap = 0x%x\n", bitmap);
	close(fd);
	return 0;
}

static int set_bitmap(int fd, u_int bitmap)
{
	if (ioctl(fd, TRACE_SET_BITMAP, &bitmap) < 0) {
		perror("ioctl failed");
		return -1;
	}
	printf("bitmap set to 0x%x\n", bitmap);
	close(fd);
	return 0;
}

static void usage(char *prog_name)
{
	printf("\nUsage: %s [CMD] /mounted/path\n", prog_name);
	printf("Controls bitmap of f/s methods to trace.\n");
	printf("When CMD is not specified, displays current value of the "
		"bitmap in hex.\n");
	printf("By default, all f/s methods will be traced.\n");
	printf("\nParameter\tDescription\n");
	printf("CMD\t\tUsed to set bitmap\n");
	printf("\t\tPossible values: all (trace all f/s methods),\n");
	printf("\t\tnone (disable tracing),\n");
	printf("\t\t0xNN (hex number representing bitmap of methods to trace)"
		"\n");
	printf("\t\tBit\tf/s method\n");
	printf("\t\t0\tcreate\n");
	printf("\t\t1\tmkdir\n");
	printf("\t\t2\tread\n");
	printf("\t\t3\trmdir\n");
	printf("\t\t4\tunlink\n");
	printf("\t\t5\twrite\n");
	printf("\t\t6\tlink\n");
	printf("\t\t7\tsymlink\n");
	printf("/mounted/path\tPath of mounted f/s\n");
}

int main(int argc, char *argv[])
{
	int fd;
	u_int val;

	if (argc < 2 || argc > 3) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	fd = open(argv[argc - 1], 0);
	if (fd < 0) {
		fprintf(stdout, "%s: failed to open %s\n", argv[0],
			argv[argc - 1]);
		exit(EXIT_FAILURE);
	}
	if (argc == 2) {
		get_bitmap(fd);
	} else {
		if (strcmp(argv[1], "all") == 0) {
			set_bitmap(fd, TRACE_ALL);
		} else if (strcmp(argv[1], "none") == 0) {
			set_bitmap(fd, TRACE_NONE);
		} else {
			if (strncmp(argv[1], "0x", 2)) {
				fprintf(stdout, "%s: invalid CMD %s\n",
					argv[0], argv[1]);
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			val = (u_int)strtol(argv[1], NULL, 0);
			set_bitmap(fd, val);
		}
	}
	exit(0);
}

