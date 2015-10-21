#ifndef _STDLIB_H
#define _STDLIB_H

#include <defs.h>
#include <sys/defs.h>
#include <sys/dirent.h>
#include <defs.h>
#include <string.h>
#include <stdio.h>
#include <pipe.h>
extern __thread int errno;

void exit(int status);
int cls();
void shutdown();


// memory
typedef uint64_t size_t;
void *malloc(size_t size);
void free(void *ptr);
void *brk(int no_of_pages);

// processes
typedef uint32_t pid_t;
pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);
int execve(const char *filename, char *const argv[], char *const envp[]);
int execvpe(const char *filename, char *const argv[], char *const envp[]);
pid_t waitpid(pid_t pid, int *status, int options);
unsigned int sleep(unsigned int seconds);
unsigned int alarm(unsigned int seconds);
void kill(int pid);
int listprocess();
void yield();
// paths
char *getcwd(char *buf, size_t size);
int chdir(const char *path);
int clear(void);

// files
typedef uint64_t ssize_t;
enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };
int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
enum { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };
typedef uint64_t off_t;
off_t lseek(int fildes, off_t offset, int whence);
int close(int fd);
int pipe(int filedes[2]);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int list_process();
// directories
#define NAME_MAX 255
typedef DIR dir;
dir *opendir(char *name);
struct dirent *readdir(dir *dir);
int closedir(dir *dir);
int listfiles(char *path,int pipe);
int echofiles(char *path,int pipe);
int catfiles(char *path,int pipe);

#endif
