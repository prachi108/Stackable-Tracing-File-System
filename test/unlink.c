#include <stdio.h>
#include <unistd.h>

int main(void)
{
	if (unlink("/mnt/trfs/cr1.txt") < 0) {
		perror("unlink failed");
		return 1;
	}
	printf("unlink succeeded\n");
	return 0;
}
