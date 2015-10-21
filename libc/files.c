/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <sys/dirent.h>
#include <sys/string.h>
#include <errno.h>

int errno;

int
open(
	const char *pathname,
	int flags)
{
	int res;
	res = (int)syscall_2(SYS_open,(uint64_t)pathname,(uint64_t)flags);

  if (-125 <= res && res < 0) {
    //errno = -res;
    res   = -1;
    errno = EACCES; 
  }
	
	return res;
}

ssize_t write(int arg1,const void *arg2, size_t arg3) {
  ssize_t res;
        if (arg1 < 0) {
                errno = EBADF;
                return 0;
        }

	res = (ssize_t )syscall_3(SYS_write,(uint64_t)arg1,(uint64_t)arg2,(uint64_t)arg3);

  /* The operating system will return a negative value on error;
 *    * wrappers return -1 on error and set the errno global variable */
  if (-125 <= res && res < 0) {
    //errno = -res;
    res   = -1;
    errno = EACCES;
  }
  return res;
}

ssize_t read(int fd, void *buf, size_t count) {

        ssize_t res;
	if (fd < 0) {
		errno = EBADF;
		return 0;
	}
	res= (ssize_t)syscall_3(SYS_read,(uint64_t)fd,(uint64_t)buf,(uint64_t)count);

	if (-125 <= res && res < 0) {
    	//errno = -res;
   		 res   = -1;
		errno = EBADF;
 	 }
        return res;
}

off_t
lseek(
	int fildes,
	off_t offset,
	int whence)
{
	off_t file_offset;
	        if (fildes < 0) {
                errno = EBADF;
                return 0;
        }

	file_offset = (off_t)syscall_3(SYS_lseek,(uint64_t)fildes,(uint64_t)offset,(uint64_t)whence);
	
	        if (-125 <= file_offset && file_offset < 0) {
        //errno = -res;
                 file_offset   = -1;
		errno = EBADF;
         }

        return file_offset;
}

int
close(
	int fd)
{
	int res;
	        if (fd < 0) {
                errno = EBADF;
                return 0;
        }

	res=(int)syscall_1(SYS_close,(uint64_t)fd);
        if (-125 <= res && res < 0) {
        //errno = -res;
                 res   = -1;
		errno = EBADF;
         }

	return res;
}

int
pipe(
	int filedes[2])
{
	int res;
	res = (int)syscall_1(SYS_pipe,(uint64_t)filedes);

        return res;
}
int
dup(
	int oldfd)
{
	        if (oldfd < 0) {
                errno = EBADF;
                return 0;
        }

	int res;
	res = (int)syscall_1(SYS_dup,(uint64_t)oldfd);
	

        return res;
}
int
dup2(
	int oldfd,
	int newfd)
{
	int res;
        if ((oldfd < 0) || (newfd <0)) {
                errno = EBADF;
                return 0;
        }

	res = (int)syscall_2(SYS_dup2,(uint64_t)oldfd,(uint64_t)newfd);
	  if (-125 <= res && res < 0) {
        //errno = -res;
                 res   = -1;
                errno = EBADF;
         }

        return res;
}
dir*
opendir(
        char *name)
{
        //int fd;
        dir *dirp;
	dirp = (dir *)syscall_1(SYS_opendir,(uint64_t)name);

       /* if ((fd = open(name, O_RDONLY | O_DIRECTORY)) == -1){
            return NULL;
	    errno = EBADF;
	}

        dirp = malloc(sizeof(dir));
        if(dirp == NULL) {
                return NULL;
		errno = ENOMEM;	
	}

        dirp->fd = fd; */

       return dirp;
}
int readdir_asm(int fd, dentry *buf, int count) {

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
    //dir *dp = (dir *)dire;
    //static int count;
    //static int pos = 0;
     dentry *dentry1;

    /* if (pos == 0) {
        count = readdir_asm(dp->fd, dentry1, sizeof(dp->buf));
    }
    if (count == -1 || count == 0) {
	    errno = EBADF;
            return  NULL;
	    
    } */
	dentry1 = (dentry *)syscall_1(SYS_readdir,(uint64_t)dire);
	

	return (dentry *)dentry1;	

//    dentry = (struct dirent *)(dp->buf + pos);
 //   pos += dentry->d_reclen;
    //if (pos >= count) {
     //   return NULL;
   // }
}

int
closedir(
        dir *dirn)
{
        dir* dirp = (dir*)dirn;
        if (dirp) {
           close(dirp->fd);
           //free(dirp);
        }

        return -1;
}
int listfiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_listfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}
int catfiles(char *path,int pipe){
	//if (path != NULL)
	//printf("%s flag %d",path,pipe);

        int ret;
        ret = (int) syscall_2(SYS_catfiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}

int echofiles(char *path,int pipe){

        int ret;
        ret = (int) syscall_2(SYS_echofiles,(uint64_t)path,(uint64_t)pipe);

        return ret;
}


