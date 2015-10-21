#include <stdlib.h>
#include <sys/defs.h>
#include <string.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
        uint64_t pid;

        if (argc >= 3){
                pid = str_to_int(argv[2]);
        } else {
                if (argc == 2)
                        printf("*****ERROR: provide pid***\n");
                else
                        printf("*****ERROR: incorrect usage****\n");

                return 0;
        }


        if (pid > 0)
                kill(pid);


        return 0;

}


