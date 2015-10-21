#include <stdlib.h>
#include <sys/defs.h>
#include <string.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
	uint64_t time;


	if (argc != 2)	
		return 0;

	time = str_to_int(argv[1]);
	if (time > 0)
		sleep(time);

	return 0;

}


	
