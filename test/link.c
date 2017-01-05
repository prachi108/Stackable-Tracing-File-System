#include <stdio.h>
#include <unistd.h>

int main(void)
{
	if (link("/mnt/trfs/cr1.txt", "/mnt/trfs/cr1_link") < 0) {
		perror("link failed");
		return 1;
	}
	printf("link succeeded\n");
	return 0;
}
