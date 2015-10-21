/* © Copyright 2015, ggehlot@cs.stonybrook.edu
	 arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
	 Everyone is permitted to copy and distribute verbatim copies
	of this license document, but changing it is not allowed.
*/

#include<stdio.h>
#include<stdlib.h>

int parse_ps1(char string[],char **token_array) {

        int array_pos=0;
        int counter=0;
        int str_pos=0;
        token_array[array_pos]=(char *) malloc(sizeof(string));

        while ((string[str_pos] == ' ') || (string[str_pos] == '\t'))
             str_pos++;

        while ( string[str_pos] != '\0' ) {

                if ( (string[str_pos] == '/') || (string[str_pos]== '\t' ) ) {

                         if (counter == 0) {
                          token_array[array_pos][counter] = string[++str_pos];
                        token_array[array_pos][++counter] =  '\0';
                        counter=0;
                        array_pos++;
                        token_array[array_pos]= (char *)malloc(sizeof(string));
                        str_pos++;
                        } else {
                                token_array[array_pos][++counter] = '\0';
                                counter=0;
                                array_pos++;
                                token_array[array_pos]= (char *)malloc(sizeof(string));
                                str_pos++;
                                token_array[array_pos][counter] = string[str_pos];
                                token_array[array_pos][++counter] =  '\0';
                                counter=0;
                                array_pos++;
                                token_array[array_pos]= (char *)malloc(sizeof(string));
                                str_pos++;

                        }

                } else {
                   token_array[array_pos][counter] = string[str_pos];
                  counter++;
                  str_pos++;
                }
        }

        if (token_array[array_pos] == NULL) {
           printf("No arguments passed");
           return 0;
        } else {
          token_array[++array_pos]=NULL;
          return 1;
        }
}
char *print_dir() {
                int size=10000;
                char *ptr = (char *)malloc(10000*sizeof(char));
		ptr = (char *)getcwd(ptr,size);
		if(ptr == NULL)
			return NULL;
			printf("ptr %s\n", ptr);
                size=0;
                while (ptr[size++] != '\0');
                char *temp=(char *)malloc(100);
                int pos=0;

                while (ptr[--size] != '/' );

                while ((temp[pos++] = ptr[size++]) != '\0');

		free(ptr);
		printf("temp %s\n", temp);
		return temp;
}

char* print_prompt(char string[]){

        char **token_array;
        int status;
        token_array = (char **)malloc(1024*sizeof(char *));
        status = parse_ps1(string, token_array);
	printf("status : %d\n", status);
        if ( status == 0 )
                return NULL;
        int pos=0;
        char* ret_str = (char *)malloc(100*sizeof(char ));
	//char *pd;
	int ret_pos = 0;
	int count = 0;
        while( token_array[pos] != NULL )
        {       /*Out of Bound error in getcwd system call, running fine on local machine
                if ((token_array[pos][0] == 'w') && (token_array[pos][1] == '\0')){
                        pd = print_dir();
			printf("%s\n", pd);
			 while(pd[count] != '\0') {
                        ret_str[ret_pos++] = pd[count];
                        count++;
                	}
			count = 0;
                        pos++;
			free(pd);
                        continue;
                } */
                //Implementation dependent on getlogin and gethostname,will be done at later stages.
               /* if ((token_array[pos][0] == 'h') && (token_array[pos][1] == '\0')) {
                        gethostname(buffer,10240);
                        printf("%s",buffer);
                        pos++;
                        continue;
                
                if ((token_array[pos][0] == 'u') && (token_array[pos][1] == '\0')){
                        printf("%s", getlogin());
                        pos++;
                        continue;
                } */
		while(token_array[pos][count] != '\0') {
			ret_str[ret_pos++] = token_array[pos][count];
			count++;
		}
		count=0;
		free(token_array[pos]);
		pos++;
		
        }
	ret_str[ret_pos++] = '$';
	ret_str[ret_pos] = '\0';


        free(token_array);

	return ret_str;
}


