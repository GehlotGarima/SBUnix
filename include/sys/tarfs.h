#ifndef _TARFS_H
#define _TARFS_H

#include <sys/dirent.h>


extern char _binary_tarfs_start;
extern char _binary_tarfs_end;
typedef struct posix_header_ustar tarfs_header;
void init_tarfs();

struct file_t* create_node(char *name,file_t *parent_node,uint64_t type,uint64_t first,uint64_t last,uint64_t inode_no);
void parse_tarfs(char *dir,int type,uint64_t first,uint64_t last);
void init_tarfs();
uint64_t power (uint64_t x, int e);

struct posix_header_ustar {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

#endif
