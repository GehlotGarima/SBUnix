#ifndef __SYS_SYSCALL_H
#define __SYS_SYSCALL_H

#include <sys/interrupts.h>
#include <sys/process.h>
#include <sys/elf.h>
#include <sys/dirent.h>
#define SYS_exit       60
#define SYS_brk        12
#define SYS_fork       57
#define SYS_getpid     39
#define SYS_getppid   110
#define SYS_execve     59
#define SYS_wait4      61
#define SYS_nanosleep  35
#define SYS_alarm      37
#define SYS_getcwd     79
#define SYS_chdir      80
#define SYS_open        2
#define SYS_read        0
#define SYS_write       1
#define SYS_lseek       8
#define SYS_close       3
#define SYS_pipe       22
#define SYS_dup        32
#define SYS_dup2       33
#define SYS_getdents   78
#define SYS_opendir    4
#define SYS_readdir    5
#define SYS_closedir   6
#define SYS_listprocess 7
#define SYS_killprocess 13
#define SYS_clearscreen 14
#define SYS_shutdown    15 
#define SYS_listfiles   16
#define SYS_catfiles    17
#define SYS_echofiles   18
#define SYS_yield       19
dentry*  sys_readdir(struct dir* dir1);
struct dir* sys_opendir(char *path);
int sys_closedir(struct dir* dir);

void syscall_handler(struct isr_regs *r);

void schdule_next(void);
uint64_t syscall_caller(uint64_t no_returned,uint64_t par1,uint64_t par2,uint64_t par3);
int sys_getpid();
int sys_getppid();
int sys_sleep(uint64_t second);
void sys_exit();
uint64_t sys_dup(uint64_t oldfd);
uint64_t sys_dup2(uint64_t oldfd, uint64_t newfd);
int sys_open(char *path, uint64_t flag );
uint64_t sys_brk(uint64_t pages);
int sys_read(uint64_t fd_count,uint64_t addr,uint64_t len);
int sys_write(uint64_t fd_count, uint64_t addr, int len);
void  sys_close(int fd_count);
uint64_t sys_getcwd(char *buf, uint64_t size);
int sys_alarm(int second);
uint64_t sys_brk(uint64_t pages);
int sys_lseek(uint64_t fd_type, int offset, int whence);
int sys_chdir(char *path);
uint64_t sys_getdents(uint64_t fd_no,uint64_t buf,uint64_t count);
dentry* sys_readdir(struct dir* dir);
int sys_closedir(struct dir* entry);
struct dir* sys_opendir(char *path);
int sys_waitpid(uint64_t childpid,uint64_t status,uint64_t options);
int sys_listprocess(int pipe);
int sys_killprocess(uint64_t pid);
int sys_clearscreen();
int sys_listfiles(char *path,int pipe);
int sys_catfiles(char *path,int pipe);
int sys_echofiles(char *path,int pipe);
int sys_shutdown();
int sys_yield();


#endif
