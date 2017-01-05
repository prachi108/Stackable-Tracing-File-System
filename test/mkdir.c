#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
	if (mkdir("/mnt/trfs/dir", 0700) < 0) {
		perror("mkdir failed");
		return 1;
	}
	printf("mkdir succeeded\n");
	if (mkdir("/mnt/trfs/dir/dir1", 0777) < 0) {
		perror("mkdir failed");
		return 1;
	}
	printf("mkdir succeeded\n");
	if (mkdir("/mnt/trfs/dir2", 0755) < 0) {
		perror("mkdir failed");
		return 1;
	}
	printf("mkdir succeeded\n");
	return 0;
}
