#include <stdio.h>
#include <unistd.h>

int main(void)
{
	if (rmdir("/mnt/trfs/dir/dir1") < 0) {
		perror("rmdir failed");
		return 1;
	}
	printf("rmdir succeeded\n");
	return 0;
}
