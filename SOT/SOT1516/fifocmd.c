/* Ejercicio: 7
Jhon Fajardo Rodas   Tecnologias
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>


char *array_commands[1024];
char *array_commandLine[1024];
char *array_paths[1024];
char path_command[2048];



int
tokenize(char *str, char *limit, char *array[])
{

	char *token, *saveptr;
	int n = 0;

	token = strtok_r(str, limit, &saveptr);
	while(token){

		array[n] = token;
		n++;
		token = strtok_r(NULL, limit, &saveptr);
	}
	return n;
}

void
tok_Path(int nAux)
{

	int j;
	int n;
	char path_aux[2048];
	for (j = 0; j < nAux; j++)
	{

		n = strlen(array_paths[j]);
		memset(path_aux, 0, 2048);
		strcat(path_aux, array_paths[j]);
		if(array_paths[j][n-1] != '/')
			strcat(path_aux, "/");
		array_paths[j] = strdup(path_aux);


	}
}

char*
cat_command(char *command, char *dir)
{
	memset(path_command, 0, 2048);
	strcat(path_command, dir);
	strcat(path_command, command);
	return path_command;
}

char*
creat_dir_Command(char *command, int n_paths)
{
	int i;
	char *dir;
	int ok = 0;

	while(!ok)
	{
		if(access(cat_command(command, array_paths[i]), X_OK) == 0)
		{
			dir = cat_command(command, array_paths[i]);
			ok = 1;
		}else{
			i++;
		}
	}

	return dir;
}



int
main(int argc, char *argv[])
{
	char *pathFifo;
	char buffer[1024];
	int nr;
	int nArgm;
	int fd, fd2;
	int x;
	char *vePath;
	int nPaths;
	int pid, pid2;
	int pipefd[2];
/*
	fd2 = open("/dev/null", 0660);
	if(fd2 < 0)
	{
		err(1, "open: /dev/null");
	}
	dup2(fd2, 2);
	close(fd2);
*/


	pathFifo = argv[1]; // guardo el path del fifo
	vePath = getenv("PATH");
	nPaths = tokenize(vePath, ":", array_paths);



	if (access(pathFifo, F_OK)== 0)

		unlink(pathFifo);


	if(mkfifo(pathFifo, 0664) < 0)
		err(1, "mkfifo");

	// guardo comandos de la linea de argumentos

	for(x=0; x < argc - 2; x++)
	{

		array_commandLine[x] = strdup(argv[x+2]);
	}
	array_commandLine[x] = NULL;

	//tokenizo $PATH y añado un "/"

	tok_Path(nPaths);

	int fdOut;
	fdOut = creat("fifocmd.out", 0660);
	if(fdOut < 0)
	{
		err(1, "creat fOut");
	}
	close(fdOut);

/*
	fd2 = open("/dev/null", 0660);
	if(fd2 < 0)
	{
		err(1, "open: /dev/null");
	}
	dup2(fd2, 2);
	close(fd2);
	*/



	for(;;)
	{
		// Abro el fifo para leer los comandos
		fd = open(pathFifo, O_RDONLY);
		if (fd < 0){
			err(1,"open file");
		}
		for(;;)
		{
			nr = read(fd, buffer, sizeof buffer);
			if(nr < 0){
				err(1, "read");
			}
			if(nr == 0)
				break;

		}
		close(fd);
		// tokenizo lo que leo del fifo para sacar los comandos y argumentos
		tokenize(buffer, "\n", array_commands);

		nArgm  = tokenize(array_commands[0], " ", array_commands);
		array_commands[nArgm] = NULL;

		if (pipe(pipefd) == -1) {
	        perror("pipe");
	        exit(EXIT_FAILURE);
	    }

		pid = fork();

		switch(pid)
		{

			case -1:

				exit(EXIT_FAILURE);

			case 0:

				dup2(pipefd[1], 1);
				close(pipefd[1]);
				close(pipefd[0]);

				fprintf(stderr, "%s\n", creat_dir_Command(array_commands[0], nPaths));
				execv(creat_dir_Command(array_commands[0], nPaths), array_commands);
				fprintf(stderr, "%s: error en el comando del fifo\n", array_commands[0]);
				exit(EXIT_FAILURE);
				// buscamos los paths totales del comando
				// ejecutamos los comandos
			default:
				wait(NULL);

		}

		pid2 = fork();

		switch(pid2)
		{
			case -1:

				exit(EXIT_FAILURE);

			case 0:

				dup2(pipefd[0], 0);
				close(pipefd[1]);
				close(pipefd[0]);


				fdOut = open("fifocmd.out", O_WRONLY|O_APPEND, 0660);
				if(fdOut < 1)
				{
					err(1, "open fifocmd.out");
				}
				dup2(fdOut, 1);
				close(fdOut);

				execv(creat_dir_Command(array_commandLine[0], nPaths), array_commandLine);
				fprintf(stderr, "%s: error en el comando de la linea de argumentos\n", array_commandLine[0]);
				exit(EXIT_FAILURE);
			default:
				close(pipefd[1]);
				close(pipefd[0]);
				wait(NULL);

		}

	}

	exit(EXIT_SUCCESS);
}
