#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv);

void _start(int argc, char* argv[]) {
	//int argc = 1;
	//char* argv[10];
	//char* envp[10];
	int res;
	res = main(argc, argv);
	exit(res);
}
