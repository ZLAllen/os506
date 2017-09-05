

//open sys flags
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001

//more to add
int open(const char* file, int flag);
int close(int fd);
int read(int fd, char* buf, int size);
int write(int fd, const char* buf, int size);
int fork();
int execvpe(char *path, char *argv[], char *envp[])
