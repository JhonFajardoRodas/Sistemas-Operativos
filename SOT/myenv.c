
/* EJERCICIO: 3 
 * 
 * Jhon Fajardo Rodas  Tecnologías*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>       
#include <unistd.h>



int 
main (int argc, char *argv[])
{
	
char *a[128];
int i;
pid_t p;
uid_t u;
	
	p = getpid();
	u = getuid();
	printf("PID : %d\n", p);
	printf("UID : %d\n", u);
	for (i = 1; i < argc; i++)
	{
			a[i] = getenv(argv[i]);
			if (a[i] != NULL){
				printf("%s : %s\n" ,argv[i], a[i]);
			}else{
				fprintf(stderr, "error %s does not exist \n",argv[i]);
			}
	}
	exit(EXIT_SUCCESS);
}
