#include <stdio.h>
#include <fcntl.h>

int main(void)
{
	if (creat("/mnt/trfs/cr1.txt",
			S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
		perror("create failed");
		return 1;
	}
	printf("create succeeded\n");
	if (creat("/mnt/trfs/cr2.txt",
			S_IRUSR | S_IWUSR) < 0) {
		perror("create failed");
		return 1;
	}
	printf("create succeeded\n");
	if (creat("/mnt/trfs/cr3.txt",
			S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) < 0) {
		perror("create failed");
		return 1;
	}
	printf("create succeeded\n");
	return 0;
}
