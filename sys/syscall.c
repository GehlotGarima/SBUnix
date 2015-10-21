#include <sys/interrupts.h>
#include <sys/vmmu.h>
#include <sys/string.h>
#include <sys/pmap.h>
#include <sys/sbunix.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include <sys/process.h>
#include <sys/dirent.h>
#include <sys/elf.h>

#define FD_MAX 100
#define PAGESIZE 4096
#define TRUE 1
#define FALSE 0
#define MAX_DIR_SIZE 30
#define RD_PIPE 2
#define WR_PIPE 1
#define NO_PIPE 0
char buffer[100];
enum { SEEK_SET = 0, SEEK_CUR = 1, SEEK_END = 2 };
enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };

extern task_struct *current;
extern task_struct *sleep_task;

char pipe_buffer[4096];
void syscall_handler(struct isr_regs *regs){

 uint64_t no_returned;
 uint64_t return_value;

        no_returned  = regs->rax;
        return_value = syscall_caller(no_returned,regs->rdi,regs->rsi,regs->rdx);
        regs->rax = return_value; /*return value in rax*/

	*((int64_t *)current->init_kern - 9) = return_value;	

//      printf("no_teruned   %d",no_returned);
//         printf("return value %d",return_val);

}

/* handler for syscall */
uint64_t syscall_caller(uint64_t no_returned,uint64_t par1,uint64_t par2,uint64_t par3){

        switch (no_returned) {

        case SYS_exit:
                sys_exit();
                return 0;
        case SYS_brk:
                return sys_brk((uint64_t)par1);
        case SYS_fork:
              return sys_fork();      
        case SYS_getpid:
                return sys_getpid();
        case SYS_getppid:
                return sys_getppid();
        case SYS_execve:
              return sys_execve((char *)par1,(char **)par2,(char **)par3);
        case SYS_wait4:
                return sys_waitpid((uint64_t) par1,(uint64_t) par2,(uint64_t) par3);
        case SYS_nanosleep:
                return sys_sleep((uint64_t)par1);
        case SYS_alarm:
                return sys_alarm((uint64_t)par1);
       case SYS_getcwd:
                return sys_getcwd((char *)par1,(uint64_t)par2);
        case SYS_chdir:
                return sys_chdir((char *)par1);
        case SYS_open:
                return sys_open((char *)par1,(uint64_t)par2);
        case SYS_read:
                return sys_read((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_write:
                return sys_write((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_lseek:
                return sys_lseek((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_close:
                sys_close((uint64_t)par1);
                return 0;
        case SYS_dup:
               return sys_dup((uint64_t)par1);
        case SYS_dup2:
                return sys_dup2((uint64_t)par1,(uint64_t)par2);
        case SYS_getdents:
                return sys_getdents((uint64_t)par1,(uint64_t)par2,(uint64_t)par3);
        case SYS_opendir:
                return (uint64_t)sys_opendir((char *)par1);
        case SYS_readdir:
                return (uint64_t)sys_readdir((struct dir *)par1);
        case SYS_closedir:
                return (uint64_t)sys_closedir((struct dir *)par1);
	case SYS_listprocess:
		return sys_listprocess((uint64_t)par1);
	case SYS_killprocess:
		return sys_killprocess((uint64_t)par1);
	case SYS_clearscreen:
		return sys_clearscreen();
	case SYS_listfiles:
		return sys_listfiles((char *)par1,(uint64_t)par2);
        case SYS_catfiles:
                return sys_catfiles((char *)par1,(uint64_t)par2);
        case SYS_echofiles:
                return sys_echofiles((char *)par1,(uint64_t)par2);
        case SYS_shutdown:
                return sys_shutdown();
        case SYS_yield:
                return sys_yield();


	
        default:
                return -1;

      }


}
/* syscall to clear the screen */
int sys_clearscreen() {

clear_console();
return 1;

}

/* syscall to get current process pid */

int sys_getpid()
{
       unsigned int pid_sys = (unsigned int) current->pid;
                return pid_sys;
}
/*syscall to get parent pid*/
int sys_getppid()
{
       unsigned int pid_sys = (unsigned int)current->ppid;
        return pid_sys;
}

/* syscall to sleep the process */
int sys_sleep(uint64_t seconds)
{
	nano_sleep(seconds);
         //set_sleep_flag();
	//set_sleep_time(seconds);
   //`return current->sleep;
	return 0;

}
char *print_state(int task_state){

	switch (task_state)  {

	case TASK_READY:
		return "READY          ";
	case TASK_RUNNING:
		return "RUNNING        "; 
	case TASK_INTERRUPTIBLE :    
		return "INTERRUPTIBLE  "; 
	case TASK_UNINTERRUPTIBLE:
		return "UNINTERRUPTIBLE";						
	case TASK_STOPPED: 
		return "STOPPED        ";
	case TASK_ZOMBIE: 
		return "ZOMBIE         ";
	case TASK_WAITING:
		return "WAITING        ";
	default:
		return "UNKNOWN        ";
	}
	return NULL;

}
/*syscall to list the running process */

int sys_listprocess(int pipe_flag) {

        int count =1;

        task_struct *temp = current;
	task_struct *head1 =  current;
	char *pipe = pipe_buffer;
        if (pipe_flag == RD_PIPE){
            //printf("read pipe enabled \n");
	    //printf("read data is %s",pipe_buffer);
        }else if (pipe_flag == WR_PIPE || pipe_flag == NO_PIPE){
        bzero(pipe_buffer,4096);

        }



	if (pipe_flag != WR_PIPE){ 
        	printf("\n     	 LIST of TASKS		");
        	printf("\n No|pid|ppid|      state      |    name     ");
	} else {
		strcpy(pipe,"\n       LIST of TASKS          \nNo|pid|ppid|      state      |    name     ");
	}
	char *temp_pipe; 

        do {
	if (pipe_flag != WR_PIPE) {
        printf("\n %d | %d | %d  | %s |%s",
                count++,temp->pid,temp->ppid,print_state(temp->task_state),temp->pname );
	} else {
		
		temp_pipe = pipe + strlen(pipe);

                strcpy(temp_pipe,"\n ");
		*temp_pipe = '\n';
		temp_pipe++;
		char *temp_p = int_to_str(count);
		strcpy(temp_pipe,temp_p);
		temp_pipe += strlen(temp_p);
		count++;

                strcpy(temp_pipe," | ");
                temp_pipe += 3;
		temp_p = int_to_str(temp->pid);
                strcpy(temp_pipe,temp_p);
		temp_pipe += strlen(temp_p);

                strcpy(temp_pipe," | ");
                temp_pipe += 3;
		temp_p = int_to_str(temp->ppid);
		strcpy(temp_pipe,temp_p);
                temp_pipe += strlen(temp_p);

		strcpy(temp_pipe,"  |  ");
		temp_pipe += 5;
		strcpy(temp_pipe,print_state(temp->task_state));
		temp_pipe += strlen(print_state(temp->task_state));

                strcpy(temp_pipe,"| ");
                temp_pipe += 2;
		strcpy(temp_pipe,temp->pname);
	}
		
	 temp = temp->next;
	
        }while (temp != head1);

	if (sleep_task != NULL ){
		temp = sleep_task;
        	head1 =  sleep_task;

	        do {
        		if (pipe_flag != WR_PIPE) {
        			printf("\n %d | %d | %d  | %s |%s",
        		        count++,temp->pid,temp->ppid,print_state(temp->task_state),temp->pname );
        		} else {

                		temp_pipe = pipe + strlen(pipe);

                		strcpy(temp_pipe,"\n ");
                		*temp_pipe = '\n';
                		temp_pipe++;
                		char *temp_p = int_to_str(count);
                		strcpy(temp_pipe,temp_p);
                		temp_pipe += strlen(temp_p);
                		count++;

                		strcpy(temp_pipe," | ");
                		temp_pipe += 3;
                		temp_p = int_to_str(temp->pid);
                		strcpy(temp_pipe,temp_p);
                		temp_pipe += strlen(temp_p);

                		strcpy(temp_pipe," | ");
                		temp_pipe += 3;
                		temp_p = int_to_str(temp->ppid);
                		strcpy(temp_pipe,temp_p);
                		temp_pipe += strlen(temp_p);

                		strcpy(temp_pipe,"  |  ");
                		temp_pipe += 5;
                		strcpy(temp_pipe,print_state(temp->task_state));
                		temp_pipe += strlen(print_state(temp->task_state));

             		        strcpy(temp_pipe,"| ");
                		temp_pipe += 2;
                		strcpy(temp_pipe,temp->pname);
        		}

         		temp = temp->next;

        	}while ((temp != head1) && (temp != NULL));

	}
	
	if ( pipe_flag != WR_PIPE){	
		printf("\n");
	}

	return 1;

}

/* syscall to wait for the child to execute */

int sys_waitpid(uint64_t childpid,uint64_t status,uint64_t options)
{
        //int wait_on_child;


        if (childpid > 0) {
                current->wait_on_child_pid = childpid;
        } else {
                current->wait_on_child_pid = 0;
        }

        current->task_state = TASK_WAITING;
	
	nano_sleep(0);
        //wait_on_child = (uint64_t) current->wait_on_child_pid;
        //return wait_on_child;

	return 0;

}

/* syscall to get the current working directory*/
uint64_t sys_getcwd(char *buf, uint64_t size)
{

        if (size < strlen(current->cur_dir)) {
           return -1;
        }
        strcpy(buf,current->cur_dir); //defined cur_dir, update it on task creation,copy from parent

        return (uint64_t)buf;

}

int sys_alarm(int second)
{
        task_struct *cur_task =current;
        cur_task->alarm = 1;
        nano_sleep(second);
        return second;
}

/* syscall to exit the current process  */
void sys_exit()
{

        task_struct *cur_task = current;

        /*
        remove task from its parent
        remve all childs of task
        */
        cur_task->task_state = TASK_STOPPED;

	exit(0);
}

/* syscall to return duplicate fd */
uint64_t sys_dup(uint64_t oldfd)
{
   task_struct *curr_task = current;
   int count = 2;

    if (curr_task->fd[oldfd] == NULL)
                return oldfd;


   while ((curr_task->fd[++count] != NULL) && count < FD_MAX);

   if (count >= MAX_FD)
             return -1;
      else {
             curr_task->fd[count] = curr_task->fd[oldfd];
             return count;
           }

}
/* syscall to replace oldfd with newfd */
uint64_t sys_dup2(uint64_t oldfd, uint64_t newfd)
{
    task_struct *current_task = current;
    if (newfd > MAX_FD || oldfd > MAX_FD)
        return -1;


   if (newfd == oldfd)
        return newfd;

    if (current_task->fd[newfd] != NULL){
        current_task->fd[newfd] = NULL;
        current_task->fd[newfd] = current_task->fd[oldfd];
    } else {
       current_task->fd[newfd] = current_task->fd[oldfd];
    }
   return newfd;


}
/* for opening files and directories */
int sys_open(char *path, uint64_t flag )
{
        file_t *node;
        file_t *node_temp;
        char *name;
        char *dirpath;
        int i=0;
        int count = 2;
        struct fd *fd1 = (struct fd*)kmalloc();
        node = root;

        dirpath = (char *)kmalloc();
        strcpy(dirpath,path);

        name = strtok(dirpath,"/"); /* get the first token */

        if (name == NULL)
                return -1;

        if (strcmp(name, "rootfs") == 0) {
                while ( name !=NULL ) {
                        node_temp = node;
                        if (strcmp(name,".") == 0 ) { /* logic to detect dot notation in addresing */
                                node = node->child[0];

                         } else if (strcmp(name,"..") == 0) {
                                node = node->child[1];
                         } else {


                        for (i=2; i < node->last ; i++) {

                                if (strcmp(name,node->child[i]->name) == 0) {
                                    node = (file_t *)node->child[i];
                                    break;
                                }
                         }
                        }

                        if (i >= node_temp->last)
                            return -1;

                         name = strtok(NULL,"/");
              }
              if ((node->type == DIRECTORY && flag == (O_RDONLY)) || (node->type == FILE))
                {
                        fd1->node = node;
                        fd1->permission =flag;
                        fd1->current = node->first;
                } else {
                        return -1;
                }

              while ((current->fd[++count] != NULL) && count < FD_MAX);

              if (count >= FD_MAX)
                        return -1;
                else {
                        current->fd[count] = fd1;
                        return count;
                }

        }
        return -1;
}

/* syscall to assign memory */
uint64_t sys_brk(uint64_t pages)
{
   uint64_t addr = current->mm->brk;
   //struct vm_struct *vma_ptr;
   struct mm_struct *mm_ptr = current->mm;
   uint64_t size;
   size = pages*PAGESIZE;
        /*vma_struct *vma = (vma_struct *)kmalloc();
        mm_ptr->current->next = vma;
        mm_ptr->current = vma;
        vma->mm = mm_ptr;
        vma->start = (uint64_t)kmalloc();
        vma->end = vma->start + size;
        vma->flags = (PERM_R | PERM_W);
        vma->type = HEAP;
        vma->file = NULL;
        vma->next = NULL; */

        mm_ptr->brk +=size;
        mm_ptr->end_data +=size;
        mm_ptr->total_vm   +=size;
        return addr;
}

/* syscall to read from file */
int sys_read(uint64_t fd_count,uint64_t addr,uint64_t len)
{

        uint64_t len_read=0;
        uint64_t len_end =0;
        uint64_t count;

//      struct task_struct *current_task = current;
        if (fd_count == stdin) {
            count = scanf_stdin((void *)addr,len);//define it
             return count;
        }

       if ((current->fd[fd_count] != NULL) && (current->fd[fd_count]->permission != O_WRONLY)) {
         len_read = current->fd[fd_count]->current;
         len_end  = current->fd[fd_count]->node->last;
        if (len > (len_end - len_read))
                len = len_end - len_read;
         current->fd[fd_count]->current +=len;
         memcpy((void *)addr,(void *)len_read,len);//define memcpy

       return len;
      }

     return -1;
}
/* sysscall to close the fd */
void  sys_close(int fd_count)
{
 current->fd[fd_count] = NULL;
}

/* syscall to write on stdin/stderror  */
int sys_write(uint64_t fd_count, uint64_t addr, int len)
{
       // int len = 0;
       // vma_struct *vma_ptr;
        if (fd_count == stdout || fd_count == stderr) {
              len =printf((char *)addr);
                return len;
        }

        return -1;

}

/* syscall to setup offset on file */
int sys_lseek(uint64_t fd_count, int offset, int whence) {


    uint64_t first =0;
    uint64_t last  = 0;
    struct fd *fd_curr = current->fd[fd_count];

        if (fd_curr->node->type == DIRECTORY) {
                offset = -1;
                return offset;
        }

        first = fd_curr->node->first;
        last = fd_curr->node->last;

        if(whence == SEEK_SET) {
                fd_curr->current = first + offset;
        } else if (whence == SEEK_CUR) {
                if (fd_curr->current +offset > last) {
                        fd_curr->current= last;
                } else {
                        fd_curr->current +=offset;
                }

        } else if (whence == SEEK_END) {
                fd_curr->current =last;

        } else {
                offset = -1;
        }

        return offset;


}

/* syscall to open directory*/
dir* sys_opendir(char *path)
{

        file_t *node;
        file_t *node_temp;
        char *name;
        char dirpath[64];
        int i=0;

        node = root;

        strcpy(dirpath,path);

        name = strtok(dirpath,"/");
       // if (name == NULL)
        //        return (dir *)NULL;
        dir* ret_dir;
	if ( name ) {

        if ( (strcmp(name,"..") == 0) || (strcmp(name,".") == 0)){ /* to detect whther . and .. are present in starting */
                node = current->cur_node;
        }
	}

        while ( name !=NULL ) {
                        node_temp = node;

                        if (strcmp(name,".") == 0 ) {
                                node = node->child[0];

                         } else if (strcmp(name,"..") == 0) {
                                node = node->child[1];
                         } else {

                             for (i=2; i < node->last ; i++) {

                                if (strcmp(name,node->child[i]->name) == 0) {
                                    node = node->child[i];
                                    break;
                                }
                              }

                             if (i == node_temp->last) {
                                return (dir *)NULL;
                             }
                       }

                         name = strtok(NULL,"/");
        }

        if (node->type == DIRECTORY) {
                ret_dir = (dir *)kmalloc();
                ret_dir->current = 2;
                ret_dir->node = node;
                return ret_dir;
        } else {
                return (dir *)NULL;
        }
}
/* file traversal to find path */
char *print_node(file_t *p_node)

{
        char node_buf[10][30];
//      char buffer[100];
        int index = 0;
        int buf_i = 0;
        char *str = buffer;
        bzero(node_buf,300);

        file_t *node = p_node;
        if (node == NULL)
                return (char *)NULL;
        while (node != root) {
        strcpy(node_buf[index],node->name);
        index++;
        node = node->child[1];
        }

        while (index >=0)
        {
                while( node_buf[index][buf_i] != '\0')
                {
                        *str = node_buf[index][buf_i];
                        buf_i++;
                        str++;
                }

                buf_i = 0;
                index--;
                if (index >= 0)
                        *str++ = '/';
        }
        str++;
        *str = '\0';

        return buffer;
}
/* syscall to change directory */
int sys_chdir(char *path)
{
        char newpath[64];
        strcpy(newpath,path);
        dir *newdir = sys_opendir(path);
	if (newdir == NULL)
		return -1;
        if (newdir->node == NULL)
                return -1;

        strcpy(current->cur_dir, print_node(newdir->node)); /* change the current working directory */

        return 1;
}

/* syscall to return dentry */
uint64_t sys_getdents(uint64_t fd_no,uint64_t buf,uint64_t count)
{     //get hold of dentry structres
     dentry *dir ;
        dir = (dentry *)buf;
     dir->inode_no = current->fd[fd_no]->inode_no;
     strcpy(dir->name,current->fd[fd_no]->node->name);

     return (uint64_t)dir;
}

/* syscall to return the dentry of directory */
dentry* sys_readdir(struct dir* dir)
{
        if ((dir->current > 0) && (dir->node->last >= 3) && (dir->current < dir->node->last))
        {
            strcpy(dir->current_dentry.name,dir->node->child[dir->current]->name);
            dir->current++;
            return &dir->current_dentry;
        } else {
                return NULL;
        }


}

/* syscall to close the direectory */
int sys_closedir(struct dir* entry)
{

    if (entry->current <= 1)
                return -1;

    if (entry->node->type == DIRECTORY ) {

        entry->node =(file_t *)NULL;
        entry->current = 0;
        return 0;
    } else {
        return -1;
   }


}

int sys_killprocess(uint64_t pid){

	kill_process(pid,1);
	return 1;
}

int sys_listfiles(char *path,int pipe) 
{
    char a[100] = { 0 };
    strcpy(a,path);
    dir *c = sys_opendir(a);
    dentry *buf;
    int flag = pipe; 
    char *ch = pipe_buffer;
  
    if (pipe == RD_PIPE){
	//printf("%s \n",pipe_buffer);
    } else if (pipe == WR_PIPE || pipe == NO_PIPE){
	bzero(pipe_buffer,4090);

    }
    while ((buf = sys_readdir(c)) != NULL)
    {   if (flag  == WR_PIPE){
                strcpy(ch,buf->name);
                ch = ch + strlen(buf->name);
                *ch = ' ';
                ch++;

        } else {
                printf("%s\n", buf->name);

        }
    }
     //printf("piped data is %s",pipe_buffer);

    return 0;
}
void print_catfiles(char *buffer){
	char *token;
	token = strtok(buffer,"\n");
	while (token != NULL){
		printf("%s \n",token);
		token = strtok(NULL,"\n");
	}


}

int sys_catfiles(char *path,int pipe)
{
	char a[100] = {0};
	//printf("%s",path);
	if (path != NULL)
		strcpy(a,path);

    if (pipe == RD_PIPE){
        //printf("%s \n",pipe_buffer);
	print_catfiles(pipe_buffer);
    } else if (pipe == WR_PIPE || pipe == NO_PIPE){
        bzero(pipe_buffer,4096);
	int fd1;
	fd1 = sys_open(a,O_RDWR);
	if (fd1 != -1)
		sys_read((uint64_t)fd1,(uint64_t)pipe_buffer,4096);
	else { 

		dir *file = sys_opendir(path);
		if (file) {
			printf("cat : %s is a directory \n",path);
		} else {
			printf("incorrect file name \n");
		}
		return 0;
	}
	if (pipe == NO_PIPE){
		print_catfiles(pipe_buffer);
		printf("\n");	
		//printf("%s \n",pipe_buffer);
	}
    }
    return 0;
}

int sys_echofiles(char *path,int pipe)
{

        //strcpy(pipe_buffer,path);
	//printf("reached here syscall %s,",a);

    if (pipe == RD_PIPE){
        printf(" %s \n",pipe_buffer);
    } else if (pipe == WR_PIPE || pipe == NO_PIPE){
        bzero(pipe_buffer,4096);
	strcpy(pipe_buffer,path);
        if (pipe == NO_PIPE)
                printf(" %s \n",pipe_buffer);
    }
    return 0;
}

int sys_shutdown(){
   

   clear_console();

   printf("\n \n \n \n \n "); 

   printf("\n==============================Shutting Down=====================================\n \n \n"); 

   printf("================================================================================\n \n \n");
   printf("================================================================================\n \n \n");

   printf("\n==============================Shut Down========================================= \n");
   kill_process(1,0);
   kill_process(2,0);
   kill_process(3,0);
   while(1); 
   return 0;
}
int sys_yield(){
	//schedule();
	//context_switch(current->next, current);	
	return 0;

}
