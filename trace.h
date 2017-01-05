#ifndef	_TRACE_H_
#define	_TRACE_H_

/* Defines format of trace record */
struct trace_record {
	/* Unique record identifier */
	u_int id;
	/* return value of f/s method */
	int ret_val;
	/* Length of remaining record */
	u_short len;
	/* type of record */
	u_char type;
	/* Info specific to each record type */
	char record_info[0];
} __attribute__((packed));

/* Record type - represents f/s method being traced */
enum record_type {
	RECORD_CREATE,
	RECORD_MKDIR,
	RECORD_READ,
	RECORD_RMDIR,
	RECORD_UNLINK,
	RECORD_WRITE,
	RECORD_LINK,
	RECORD_SYMLINK,
};

/* Information to replay create */
struct create_info {
	u_short mode;
	char pathname[0];
};

/* Information to replay mkdir */
struct mkdir_info {
	u_short mode;
	char pathname[0];
};

/* Information to replay read */
struct read_info {
	loff_t pos;
	size_t count;
	char pathname[0];
} __attribute__((packed));

/* Information to replay write */
struct write_info {
	loff_t pos;
	size_t count;
	u_short pathname_len;
	char pathname_buf[0];
} __attribute__((packed));

/* Information to replay unlink */
struct unlink_info {
	char pathname[0];
};

/* Information to replay rmdir */
struct rmdir_info {
	char pathname[0];
};

/* Information to replay link */
struct link_info {
	u_short old_pathname_len;
	char pathnames[0];
};

/* Information to replay symlink */
struct symlink_info {
	u_short target_name_len;
	char pathnames[0];
};

/* Defines which f/s methods to trace */
enum trace_bitmap {
	TRACE_NONE,
	TRACE_CREATE = 1 << 0,
	TRACE_MKDIR = 1 << 1,
	TRACE_READ = 1 << 2,
	TRACE_RMDIR = 1 << 3,
	TRACE_UNLINK = 1 << 4,
	TRACE_WRITE = 1 << 5,
	TRACE_LINK = 1 << 6,
	TRACE_SYMLINK = 1 << 7,
	TRACE_ALL = 0xffffffff
};

#define TRACE_IOC_MAGIC	't'
#define	TRACE_SET_BITMAP	_IOW(TRACE_IOC_MAGIC, 0, u_int *)
#define	TRACE_GET_BITMAP	_IOR(TRACE_IOC_MAGIC, 1, u_int *)
#endif	/* not _TRACE_H_ */
