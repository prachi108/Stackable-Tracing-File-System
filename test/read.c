#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE	16000
int main(void)
{
	int fp;
	char buf[BUF_SIZE];

	fp = open("/mnt/trfs/workqueue.txt", O_RDONLY);
	if (fp < 0) {
		perror("open failed");
		return 1;
	}

	read(fp, buf, 100);
	read(fp, buf, 1000);
	close(fp);

	fp = open("/mnt/trfs/cr1.txt", O_RDONLY);
	if (fp < 0) {
		perror("open failed");
		return 1;
	}

	read(fp, buf, 100);
	read(fp, buf, 1000);
	close(fp);
	return 0;
}
