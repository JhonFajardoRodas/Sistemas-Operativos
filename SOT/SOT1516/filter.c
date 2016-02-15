/* Ejercicio: 6
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

struct dirent *reg_dir;
struct stat s;
char *aux[2048];
char path_command[2048];


char*
cat_command(char *command, char *dir)
{
	memset(path_command, 0, 2048);
	strcat(path_command, dir);
	strcat(path_command, command);
	return path_command;
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
	}else {
		return NULL;
	}

}

void
changeIn(char *file)
{
	int fd;
	fd = open(file, O_RDONLY);
	if(fd < 1)
		fprintf(stderr, "%s\n", "Error to open file");
	dup2(fd, 0);
	close(fd);
}

void
changeOut(char *file)
{
	int fd2;
	fd2 = creat(file, 0664);
	if (fd2 < 0) {
		err(1, "creat afile");
		}
	dup2(fd2,1);
	close(fd2);

}

void
ejecComando(char *comando, char *argm[])
{
	char *path_comando = NULL;
	path_comando = ok_command(comando);
	execv(path_comando, argm);
	exit(EXIT_FAILURE);
}
void
ejecGrep(char *argumento, char *file)
{
	char *argv[1024];
	argv[0] = "grep";
	argv[1] = argumento;
	argv[2] = file; // fichero
	argv[3] = NULL;
	char *path_comando = NULL;
	path_comando = ok_command("grep");

	execv(path_comando, argv);
	exit(EXIT_FAILURE);
}

int
searcher(char *path)
{

	DIR *d;
	int ok;
	int n=0;
	char path_new[2048];

	ok = stat(path, &s);
	if (ok < 0){
		exit(EXIT_FAILURE);
	}
	if((s.st_mode & S_IFMT) == S_IFDIR)
	 {
		d = opendir(path);
		if(d == NULL)
			exit(EXIT_FAILURE);
		for(;;)
		{
			reg_dir=readdir(d);
			if (reg_dir == NULL)
				break;
			if (reg_dir->d_name[0] == '.')
			{
				continue;
			}else{
				memset(path_new, 0, 2048);
				strcat(path_new, path);
				strcat(path_new, "/");
				strcat(path_new, reg_dir->d_name);

				ok=stat(path_new, &s);
				if(ok<0)
					exit(EXIT_FAILURE);
				if((s.st_mode & S_IFMT) == S_IFREG)
				{
					aux[n] = strdup(path_new);
					n++;
				}
			}
		}
	}

	return n;
}


int
main(int argc, char *argv[])
{

	char *argumentos[1024];
	char *cwd;
	char path[1024];
	char *command;
	int pid, pid2;
	int nFiles;
	command = argv[2];
	
	if (argc < 3)
		fprintf(stderr, "%s\n", "Usage error: Number of arguments is invalid");

	int i, j;

	for (i = 0; i < argc - 2; i++)
	{
		argumentos[i] = strdup(argv[i + 2]);

	}
	
	argumentos[i] = NULL;

	cwd = getcwd(path, 64);
	nFiles = searcher(cwd);
	
	for (j= 0; j < nFiles; j++)
	{
		pid = fork();
		switch(pid)
		{
			case -1:
				err(1, "fork failed");
				break;
			case 0:
				
				changeIn(aux[j]);
				changeOut("fileOut");
				
				ejecComando(command, argumentos);
				exit(EXIT_SUCCESS);
			default:
				wait(NULL);

		}
		

		pid2 = fork();
		switch(pid2)
		{
			case -1:
				err(1, "fork failed");
				break;
			case 0:
				ejecGrep(argv[1], "fileOut");

				exit(EXIT_SUCCESS);
			default:
				wait(NULL);

		}
		

	}
	exit(EXIT_SUCCESS);
}
