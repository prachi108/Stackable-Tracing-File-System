#include <stdio.h>
#include <unistd.h>

int main(void)
{
	if (symlink("/home/student", "/mnt/trfs/sl1_link") < 0) {
		perror("symlink failed");
		return 1;
	}
	printf("symlink succeeded\n");
	if (symlink("workqueue.txt", "/mnt/trfs/wq_sl1_link") < 0) {
		perror("symlink failed");
		return 1;
	}
	printf("symlink succeeded\n");
	return 0;
}

