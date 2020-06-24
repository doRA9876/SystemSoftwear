#define S_IFMT  00170000
#define S_IFSOCK 0140000 /* unix domain socket */
#define S_IFLNK  0120000 /* symbolic link */
#define S_IFREG  0100000 /* normal file */
#define S_IFBLK  0060000 /* block device */
#define S_IFDIR  0040000 /* directly */
#define S_IFCHR  0020000 /* character device */
#define S_IFIFO  0010000 /* named pipe */
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)  /* symbolic link */
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)  /* normal file */
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)  /* directly */
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)  /* character device */
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)  /* block device */
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)  /* named pipe */
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK) /* unix domain socket */


#define S_ISUID 0004000 /* setuid */
#define S_ISGID 0002000 /* setgid */
#define S_ISVTX 0001000 /* sticky bit */
#define S_IRUSR 00400 /* limitation: user, Read */
#define S_IWUSR 00200 /* limitation: user, Write */
#define S_IXUSR 00100 /* limitation: user, eXecute */
#define S_IRGRP 00040 /* limitation: group, Read */
#define S_IWGRP 00020 /* limitation: group, Write */
#define S_IXGRP 00010 /* limitation: group, eXecute */
#define S_IROTH 00004 /* limitation: other, Read */
#define S_IWOTH 00002 /* limitation: other, Write */
#define S_IXOTH 00001 /* limitation: other, eXecute */
