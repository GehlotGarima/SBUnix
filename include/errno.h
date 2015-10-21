#ifndef _ERRNO_H
#define _ERRNO_H

extern int errno;

#define ENOENT           1      /* file or directory  not found */
#define EIO              2      /* Input/output error */
#define EBADF            3      /* Bad file number */
#define ECHILD           4      /* No child processes */
#define EAGAIN           5     /* Try again */
#define ENOMEM           6      /* Out of memory */
#define EACCES           7      /* Permission denied */
#define EFAULT           8      /* Bad-address */
#define ESRCH            9      /* No such process */
#endif
