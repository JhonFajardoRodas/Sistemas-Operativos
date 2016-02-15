// Ejercicio: 8
// Jhon Fajardo Rodas 			Tecnologías

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>       
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

char buffer[1024];


//int
/*creat_file(char *file){

	int fd;

	fd = creat(file,0660);
	if (fd < 0){
		err(1, "file");
	}
	close(fd);
	return fd;
}*/

void
write_file(int out_sts, int n_read)
{
	int nw;
	

	nw = write(out_sts, buffer, n_read);
	if (nw != n_read){
		err(1, "write file");
	}

}

void
file_execute(int pos[2])
{
	int pid;


	pid= fork();
	switch(pid)
	{
		case -1:
			err(1, "fork failed");
			break;
		case 0:
		dup2(pos[1],1);
		close(pos[0]);
		close(pos[1]);
		execlp("gunzip","gunzip", NULL);
		exit(EXIT_FAILURE);

	}

}
	


int
main(int argc, char *argv[])
{
char *file;
int pos_pipe[2];
int nr;
int fd_file;


	
	file = argv[1];

	if(pipe(pos_pipe) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	
	file_execute(pos_pipe);

	fd_file=creat(argv[1],0664);
	if (fd_file<0)
		err(1,"%s\n",argv[1]);
	
	close(pos_pipe[1]);

	for(;;)
	{
		nr = read(pos_pipe[0], buffer, strlen(file));
		if (nr==0)
			break;
		if(nr < 0){
			err(1, "read file");
		}
		write_file(1, nr);
		write_file(fd_file, nr);
	
	

	}
	close(fd_file);
	close(pos_pipe[0]);
	wait(NULL);


	exit(EXIT_SUCCESS);

}