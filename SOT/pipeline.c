// Jhon Fajardo Rodas 	Tecnologias
// Ejercicio : 9

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

int n_pipes;
char path_aux[2048];



void
creat_pipes(int pos[50][2])
{
	int i;


	for(i = 0; i < n_pipes; i++){
		if (pipe(pos[i]) == -1){

			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
}



void
close_pipes(int pos[50][2])
{
	int j;
	for(j = 0; j < n_pipes; j++){
		close(pos[j][1]);
		close(pos[j][0]);
	}
}


int
open_file_in(char *file)
{

	int fd;
	fd = open(file, O_RDONLY);
	if (fd < 0){
		err(1, "open file");
	}
	dup2(fd, 0);
	close(fd);
	return fd;
}

int
open_file_out(char *file)
{

	int fd;
	fd = creat(file,0660);
	if (fd < 0){
		err(1, "open file");
	}
	dup2(fd, 1);
	close(fd);

	return fd;
}
char*
cat_command(char *command, char *dir)
{
	
	memset(path_aux, 0, 2048);
	strcat(path_aux, dir);
	strcat(path_aux, command);
	fprintf(stderr, "%s\n", command);
	fprintf(stderr, "%s\n", path_aux);
	return path_aux;
}

char*
ok_command(char *command)
{

	char dir_1[] = "/bin/";
	char dir_2[] = "/usr/bin/";
	char dir_3[] = "/usr/local/bin/";
	

	if (access(cat_command(command, dir_1), X_OK)==0){
		return cat_command(command, dir_1);
	}else if(access(cat_command(command, dir_2), X_OK)==0){
		return cat_command(command, dir_2);
	}else if (access(cat_command(command, dir_3), X_OK)==0){
		return cat_command(command, dir_3);
	}else{
		return NULL;
	}

}

void 
creat_fork(char *array[], int pos[50][2], int n_commmands, char *file_in, char *file_out)
{

	int i;
	int pid;
	//char *path_ok[1024];
	char *path_command = NULL;
	int fd_in;
	int fd_out;
	for(i = 0; i < n_commmands; i++)
	{

		pid = fork();
		switch(pid)
		{
			case -1:
				err(1, "fork failed");
				break;
			case 0:
				if (n_commmands == 1){
					fd_in = open_file_in(file_in);
					fd_out = open_file_out(file_out);
				}

				if (n_commmands > 1){
						fd_in = open_file_in(file_in);
						fd_out = open_file_out(file_out);
						
					
						if(i == 0){
							dup2(fd_in, 0);
							dup2(pos[0][1], 1);
							
						}else if(i == n_commmands - 1){

							dup2(pos[i-1][0], 0);
							dup2(fd_out, 1);
							
						}else{
							
							dup2(pos[i-1][0], 0);
							dup2(pos[i][1],1);
							
						}
					close_pipes(pos);
				}
				//fprintf(stderr, "path del comando : %s\n", path_command);
				path_command = ok_command(array[i]);

				execl(path_command, array[i], NULL);
				exit(EXIT_FAILURE);



		}
	}

}


int
main(int argc, char *argv[])
{

char *file_in;
char *file_out;
int pos_pipe[50][2];
char *array[1024];
int  x, f;
int n_commmands = 0;



	for(f = 1; f < argc; f++)
	{
		if (strcmp (argv[f], "-i") == 0){
			file_in = argv[f+1];
			f++;
		}else if (strcmp(argv[f], "-o") == 0){
			file_out = argv[f+1];
			f++;
		}else{
			array[n_commmands] = argv[f];
			printf(" Comandos : %s\n", array[n_commmands]);
			n_commmands ++;
		}
	}
	
	n_pipes = n_commmands-1;
	
	if (n_commmands > 1){
		creat_pipes(pos_pipe);
		creat_fork(array, pos_pipe, n_commmands, file_in, file_out);
	}else if (n_commmands == 1){

		creat_fork(array, NULL, n_commmands, file_in, file_out);
	}
	close_pipes(pos_pipe);
	for(x = 0; x < n_commmands; x++)
	{
		wait(NULL);
	}
	

	exit(EXIT_SUCCESS);
}