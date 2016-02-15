//Jhon Fajardo Rodas	Tecnologias
//Ejercicio: 10



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
time_t t;
char *hora,*saveptr;

void
handler(int s)
{
	time(&t);	
	hora = strtok_r(ctime(&t), "\n", &saveptr);
	fprintf(stderr, "%s: Time out, not events\n", hora);
	alarm(5);
}




int
main(int argc, char* argv[])
{
	char buf[1024];
	int fd, nr;
	

	

	if(access(("/tmp/logger"), F_OK)==0){
		if (unlink("/tmp/logger") < 0) {
			err(1, "/tmp/logger");
		}
		
	}

	if(mkfifo("/tmp/logger", 0664) < 0)
		err(1, "mkfifo");
	signal(SIGALRM, handler);
	for(;;)
	{
		time(&t);
		hora = strtok_r(ctime(&t), "\n", &saveptr);
	
		fprintf(stderr, "%s: Waiting for clients \n", hora);
		fd = open("/tmp/logger", O_RDONLY);
		if(fd < 0) {
			err(1, "open");
		}
		time(&t);
		
		hora = strtok_r(ctime(&t), "\n", &saveptr);
		fprintf(stderr, "%s: Ready to read events \n", hora);
		
		for(;;){
			alarm(5);

			nr = read(fd, buf, sizeof buf - 1);
			if(nr < 0){
				err(1, "read");
			}
			
			if(nr == 0){
				break;
			}
			buf[nr] = 0;
			time(&t);
			hora = strtok_r(ctime(&t), "\n", &saveptr);
			fprintf(stderr, "%s: %s",hora, buf);
			
		}
		alarm(0);
		close(fd);
		//alarm(0);
	}
	exit(0);
}
