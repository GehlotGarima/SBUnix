#include <sys/vmmu.h>
#include <sys/pmap.h>
#include <sys/string.h>
#include <sys/dirent.h>
#include <sys/tarfs.h>
#include <sys/sbunix.h>

uint64_t power (uint64_t x, int e) {

    if (e == 0) return 1;

    return x * power(x, e-1);
}

/* function to parse individually detected files in tarfs */
void parse_tarfs(char *path,int type,uint64_t first,uint64_t last)
{
        file_t *node_temp;
	file_t *node_current;
	file_t *new_temp_node;
        char *name;
        char *dirpath;
        int i=0;
        //int count = 2;

        dirpath = (char *)kmalloc();
        strcpy(dirpath,path);
	node_current = root->child[2];

        name = strtok(dirpath,"/");

        if (name == NULL)
                return ;

        while ( name !=NULL ) {
              node_temp = node_current;

             for (i=2; i < node_current->last ; i++) {
                    if (strcmp(name,node_current->child[i]->name) == 0) {
                            node_current = (file_t *)node_current->child[i];
                            break;
                     }
             }

             if (i == node_temp->last){
			new_temp_node = create_node(name,node_current,type,first,last,0);
			node_current->child[node_current->last] = new_temp_node;
			node_current->last +=1;
		}
                         name = strtok(NULL,"/");
       }

}
	

/* intiliaze tarfs */
void init_tarfs()
{
	file_t *node;

	root = (file_t *)kmalloc();
	root->type = DIRECTORY;
	root->first = 0;
	root->last = 2;
	root->current = 0;
	root->child[0] = root;
	root->child[1] = root;
	root->inode_no = 0;
	/*first node is "/" */
	strcpy(root->name,"/");	
	/*second node is rootfs */ 
	node = create_node("rootfs",root,DIRECTORY,0,2,0);
	root->last += 1;
	root->child[2]=node;

	uint64_t decimal;
        struct posix_header_ustar *head = (struct posix_header_ustar *)&_binary_tarfs_start;


        uint32_t *end = (uint32_t *)head;

	/* go till null is reached */	
	while(*end++ || *end++ || *end){
                        int j, k=0;
                        decimal=0;
                        for(j=10;j>=0;--j) /* detecting the size of file */ 
                                decimal = decimal + (head->size[j]-48) * power(8,k++);
                                if(decimal%512!=0){
                                        decimal=(decimal/512)*512;
                                        decimal+=512;
                        }
		
		if (strcmp(head->typeflag,"5") == 0){
                        parse_tarfs(head->name,DIRECTORY,0,2);
                } else {
                        parse_tarfs(head->name,FILE,(uint64_t)(head+1),(uint64_t)((void *)head + decimal + sizeof(struct posix_header_ustar)));

                }


                        head = (struct posix_header_ustar *)((uint64_t)head + decimal + sizeof(struct posix_header_ustar));
                        end = (uint32_t *)head;
                }



}

/* creating node on detecting file */
struct file_t* create_node(char *name,file_t *parent_node,uint64_t type,uint64_t first,uint64_t last,uint64_t inode_no) 
{    
	file_t *new_node = (file_t *)kmalloc();
	new_node->type = type;
	new_node->first = first;
	new_node->last =last; 
	new_node->current = first;

	new_node->child[0] = new_node;
	new_node->child[1] = parent_node;

	strcpy(new_node->name,name);
	new_node->inode_no = inode_no;

	return new_node;

}

