#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "trace.h"

#define NR_PARAMS 1		/* One fixed param TFILE */

char *record_type[] = {
	"create",
	"mkdir",
	"read",
	"rmdir",
	"unlink",
	"write",
	"link",
	"symlink",
};

/* Displays program usage */
static void usage(char *prog_name)
{
	printf("\nUsage: %s [-ns] TFILE\n", prog_name);
	printf("Displays records in TFILE and replays them\n");
	printf("\nOption\tDescription\n");
	printf("-n\tOnly display records, do not replay them\n");
	printf("-s\tstrict mode (abort replaying as soon as a deviation "
		"occurs)\n");
}

/*
 * Displays info specific to ->create
 * @ci create info
 */
static void display_create(struct create_info *ci)
{
	printf("Arguments to create:\n");
	printf("\tpathname: %s\n", ci->pathname);
	printf("\tmode: %o\n", ci->mode);
}

/*
 * Displays info specific to ->mkdir
 * @mi mkdir info
 */
static void display_mkdir(struct mkdir_info *mi)
{
	printf("Arguments to mkdir:\n");
	printf("\tpathname: %s\n", mi->pathname);
	printf("\tmode: %o\n", mi->mode);
}

/*
 * Displays info specific to ->read
 * @ri read info
 */
static void display_read(struct read_info *ri)
{
	printf("Arguments to read:\n");
	printf("\tpathname: %s\n", ri->pathname);
	printf("\tcount: %lu\n", ri->count);
	printf("\tpos: %ld\n", ri->pos);
	printf("Following syscalls would be called to replay read:\n");
	printf("\topen(%s, O_RDONLY)\n", ri->pathname);
	printf("\tlseek(fd, %ld, SEEK_SET)\n", ri->pos);
	printf("\tread(fd, buf, %lu)\n", ri->count);
}

/*
 * Displays info specific to ->rmdir
 * @ri rmdir info
 */
static void display_rmdir(struct rmdir_info *ri)
{
	printf("Arguments to rmdir:\n");
	printf("\tpathname: %s\n", ri->pathname);
}

/*
 * Displays info specific to ->unlink
 * @ui unlink info
 */
static void display_unlink(struct unlink_info *ui)
{
	printf("Arguments to unlink:\n");
	printf("\tpathname: %s\n", ui->pathname);
}

/*
 * Displays info specific to ->write
 * @wi write info
 */
static void display_write(struct write_info *wi)
{
	size_t i = 0;

	printf("Arguments to write:\n");
	printf("\tpathname: %s\n", wi->pathname_buf);
	printf("\tbuf: ");
	for (i = 0; i < wi->count; i++)
		printf("%c", wi->pathname_buf[wi->pathname_len + i]);
	printf("\n");
	printf("\tcount: %lu\n", wi->count);
	printf("\tpos: %ld\n", wi->pos);
	printf("Following syscalls would be called to replay write:\n");
	printf("\topen(%s, O_WRONLY)\n", wi->pathname_buf);
	printf("\tlseek(fd, %ld, SEEK_SET)\n", wi->pos);
	printf("\twrite(fd, buf, %lu)\n", wi->count);
}

/*
 * Displays info specific to ->link
 * @li link info
 */
static void display_link(struct link_info *li)
{
	printf("Arguments to link:\n");
	printf("\toldpath: %s\n", li->pathnames);
	printf("\tnewpath: %s\n", li->pathnames + li->old_pathname_len);
}

/*
 * Displays info specific to ->symlink
 * @si symlink info
 */
static void display_symlink(struct symlink_info *si)
{
	printf("Arguments to symlink:\n");
	printf("\ttarget: %s\n", si->pathnames);
	printf("\tlinkpath: %s\n", si->pathnames + si->target_name_len);
}

/*
 * Displays a single record
 */
static void display_record(struct trace_record *tr, char *record_info)
{
	printf("\n");
	printf("Record ID: %u\n", tr->id);
	printf("Record type: %s\n", record_type[tr->type]);
	switch (tr->type) {
	case RECORD_CREATE:
		display_create((struct create_info *)record_info);
		break;
	case RECORD_MKDIR:
		display_mkdir((struct mkdir_info *)record_info);
		break;
	case RECORD_READ:
		display_read((struct read_info *)record_info);
		break;
	case RECORD_RMDIR:
		display_rmdir((struct rmdir_info *)record_info);
		break;
	case RECORD_UNLINK:
		display_unlink((struct unlink_info *)record_info);
		break;
	case RECORD_WRITE:
		display_write((struct write_info *)record_info);
		break;
	case RECORD_LINK:
		display_link((struct link_info *)record_info);
		break;
	case RECORD_SYMLINK:
		display_symlink((struct symlink_info *)record_info);
		break;
	}
	printf("Return value of traced method: %d\n", tr->ret_val);
}

/*
 * Replays ->create
 * @ci create info
 * returns 0 on success, -errno on failure
 */
static int replay_create(struct create_info *ci)
{
	int fd;

	fd = creat(ci->pathname, ci->mode);
	if (fd < 0) {
		perror("create failed");
		return -errno;
	}
	close(fd);
	return 0;
}

/*
 * Replays ->mkdir
 * @mi mkdir info
 * returns 0 on success, -errno on failure
 */
static int replay_mkdir(struct mkdir_info *mi)
{
	if (mkdir(mi->pathname, mi->mode) < 0) {
		perror("mkdir failed");
		return -errno;
	}
	return 0;
}

/*
 * Replays ->read
 * @ri read info
 * returns no of bytes read on success, -errno on failure
 */
static int replay_read(struct read_info *ri)
{
	int fd, ret;
	char *buf;

	fd = open(ri->pathname, O_RDONLY);
	if (fd < 0) {
		perror("open failed");
		ret = -errno;
		goto end;
	}
	if (lseek(fd, ri->pos, SEEK_SET) != ri->pos) {
		perror("lseek failed");
		ret = -errno;
		goto close_fd;
	}
	buf = malloc(ri->count);
	if (!buf) {
		fprintf(stderr, "failed to alloc buf for read\n");
		ret = -ENOMEM;
		goto close_fd;
	}
	ret = read(fd, buf, ri->count);
	free(buf);
close_fd:
	close(fd);
end:
	return ret;
}

/*
 * Replays ->rmdir
 * @ri rmdir info
 * returns 0 on success, -errno on failure
 */
static int replay_rmdir(struct rmdir_info *ri)
{
	if (rmdir(ri->pathname) < 0) {
		perror("rmdir failed");
		return -errno;
	}
	return 0;
}

/*
 * Replays ->unlink
 * @ui unlink info
 * returns 0 on success, -errno on failure
 */
static int replay_unlink(struct unlink_info *ui)
{
	if (unlink(ui->pathname) < 0) {
		perror("unlink failed");
		return -errno;
	}
	return 0;
}

/*
 * Replays ->write
 * @wi write info
 * returns no of bytes written on success, -errno on failure
 */
static int replay_write(struct write_info *wi)
{
	int fd, ret;

	fd = open(wi->pathname_buf, O_WRONLY);
	if (fd < 0) {
		perror("open failed");
		ret = -errno;
		goto end;
	}
	if (lseek(fd, wi->pos, SEEK_SET) != wi->pos) {
		perror("lseek failed");
		ret = -errno;
		goto close_fd;
	}
	ret = write(fd, wi->pathname_buf + wi->pathname_len, wi->count);
close_fd:
	close(fd);
end:
	return ret;
}

/*
 * Replays ->link
 * @li link info
 * returns 0 on success, -errno on failure
 */
static int replay_link(struct link_info *li)
{
	if (link(li->pathnames, li->pathnames + li->old_pathname_len) < 0) {
		perror("link failed");
		return -errno;
	}
	return 0;
}

/*
 * Replays ->symlink
 * @si symlink info
 * returns 0 on success, -errno on failure
 */
static int replay_symlink(struct symlink_info *si)
{
	if (symlink(si->pathnames, si->pathnames + si->target_name_len) < 0) {
		perror("symlink failed");
		return -errno;
	}
	return 0;
}

/* Replays a specific record */
static int replay_record(struct trace_record *tr, void *record_info)
{
	int ret;

	switch (tr->type) {
	case RECORD_CREATE:
		ret = replay_create((struct create_info *)record_info);
		break;
	case RECORD_MKDIR:
		ret = replay_mkdir((struct mkdir_info *)record_info);
		break;
	case RECORD_READ:
		ret = replay_read((struct read_info *)record_info);
		break;
	case RECORD_UNLINK:
		ret = replay_unlink((struct unlink_info *)record_info);
		break;
	case RECORD_RMDIR:
		ret = replay_rmdir((struct rmdir_info *)record_info);
		break;
	case RECORD_WRITE:
		ret = replay_write((struct write_info *)record_info);
		break;
	case RECORD_LINK:
		ret = replay_link((struct link_info *)record_info);
		break;
	case RECORD_SYMLINK:
		ret = replay_symlink((struct symlink_info *)record_info);
		break;
	}
	return ret;
}

int main(int argc, char *argv[])
{
	struct trace_record tr;
	int fp, ret = 0, opt;
	char *record_info;
	/* By default, replay is on and strict mode is off */
	u_char replay = 1, strict_mode = 0;

	while ((opt = getopt(argc, argv, "ns")) != -1) {
		switch (opt) {
		case 'n':
			replay = 0;
			break;
		case 's':
			strict_mode = 1;
			break;
		default:
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if (optind + NR_PARAMS > argc) {
		fprintf(stderr, "%s: TFILE not specified\n", argv[0]);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (optind + NR_PARAMS < argc) {
		fprintf(stderr, "%s: invalid parameter %s\n", argv[0],
			argv[optind + NR_PARAMS]);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	fp = open(argv[optind], O_RDONLY);
	if (fp < 0) {
		fprintf(stderr, "%s: error opening trace file %s\n", argv[0],
			argv[optind]);
		exit(EXIT_FAILURE);
	}
	while ((ret = read(fp, &tr, sizeof(struct trace_record)))) {
		if (ret != sizeof(struct trace_record)) {
			fprintf(stderr,
				"%s: error reading record from trace file\n",
				argv[0]);
			exit(EXIT_FAILURE);
		} else {
			record_info = malloc(tr.len);
			if (!record_info) {
				fprintf(stderr,
					"%s: failed to alloc record info\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			ret = read(fp, record_info, tr.len);
			if (ret != tr.len) {
				fprintf(stderr,
					"%s: error reading record info\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			display_record(&tr, record_info);
			if (replay) {
				ret = replay_record(&tr, record_info);
				printf("Return value of replay: %d\n", ret);
				if (ret != tr.ret_val) {
					printf("Replay failed\n");
					if (strict_mode)
						exit(EXIT_FAILURE);
				} else {
					printf("Replay succeeded\n");
				}
			}

			free(record_info);
		}
	}
	close(fp);
	exit(EXIT_SUCCESS);
}
