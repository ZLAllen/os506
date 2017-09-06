//open sys flags
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001


//directory

struct linux_dirent {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned long d_reclen;
    char          d_name[];
};
