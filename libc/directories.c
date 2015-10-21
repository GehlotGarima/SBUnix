/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <sys/syscall.h>
#include <syscall.h>
#define BUFSIZE 1024
/*
dir* opendir(char *name)
{
	dir* res ;
	res = (dir *)syscall_1(SYS_opendir,(uint64_t)name);
        return res;
}

dirent* readdir(dir *directory)
{

	dirent* res;
	res = (dirent *)syscall_1(SYS_readdir,(uint64_t)directory);
	return res;


}

int closedir(dir *directory)
{
	int res;
	res = syscall_1(SYS_close,(uint64_t)directory);
	return res;

}

typedef struct {
	int fd;
	char buf[BUFSIZE];
	
}DIR;

dir*
opendir(
	char *name)
{
	int fd;
	dir *dirp;

	if ((fd = open(name, O_RDONLY | O_DIRECTORY)) == -1)
            return NULL;

	dirp = malloc(sizeof(dir));
	if(dirp == NULL)
		return NULL;	

	dirp->fd = fd;

       return dirp;
} 
int readdir_asm(int fd, char *buf, int count) {

        int res = 0;
	res =(int)syscall_3(SYS_getdents,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);

        if (-125 <= res && res < 0) {
                res = -1;
        }
        return res;
}

struct dirent*
readdir(dir *dire)
{
    dir *dp = (dir *)dire;
    static int count;
    static int pos = 0;
    struct dirent *dentry;

    if (pos == 0) {
        count = readdir_asm(dp->fd, dp->buf, sizeof(dp->buf));
    }
    if (count == -1 || count == 0) {
            return  NULL;
    }
    
    dentry = (struct dirent *)(dp->buf + pos);
    pos += dentry->d_reclen;
    if (pos >= count) {
        return NULL;
    }
    return dentry;
}

int
closedir(
	void *dir)
{
	DIR* dirp = (DIR*)dir;
	if (dirp) {
           close(dirp->fd);
           //free(dirp);
	}

	return -1;
}
*/


