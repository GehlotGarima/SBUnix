#ifndef DIRENT_H
#define DIRENT_H

#include <sys/defs.h>

#define FILE 1
#define DIRECTORY 2

#define RD_ONLY 1
#define WR_ONLY 2
#define RD_WR  3
#define stdin 0
#define stdout 1
#define stderr 2

typedef struct file_t file_t;
typedef struct fd fd;
typedef struct dirent dentry;
typedef struct dir dir;
typedef struct dirent dirent;
struct file_t {
        uint64_t first;
        uint64_t last;
        uint64_t current;
        file_t* child[20];
        char name[20];
        int type;
        uint64_t inode_no;
};

struct fd {
        uint64_t current;
        uint64_t permission;
        uint64_t inode_no;
        file_t* node;
};

struct dirent {
        uint64_t inode_no;
        char name[30];
};

struct dir {
        file_t* node;
        uint64_t current;
        dentry current_dentry;
	int fd;
	char buf[20];
};
typedef dir DIR;
/*// directories
#define NAME_MAX 255
dir *opendir(const char *name);
struct dirent *readdir(dir *dir);
int closedir(dir *dir);
*/

file_t* root;

#endif
