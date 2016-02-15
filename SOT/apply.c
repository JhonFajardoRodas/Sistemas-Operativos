// Jhon Fajardo Rodas
// Ejercicio: 7



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

struct dirent *reg_dir;
struct stat s;
char path_command[2048];
char path_new[2048];
char buffer[2048];
char path_file[2048];
char path_file_output[2048];
int found_apply;

void
open_file_in(char *file)
{

	int fd;
	fd = open(file, O_RDONLY);
	if (fd < 0){
		err(1, "open file");
	}
	dup2(fd, 0);
	close(fd);
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



int
search_txt(char *path)
{


	char *key;
	key = strstr(path, ".txt");
	if((key != NULL) && strlen(key)==strlen(".txt")){
		return 1;
	}else{
		return 0;
	}
}

int
search_output(char *path)
{


	char *key;
	key = strstr(path, "apply.output");
	if((key != NULL) && strlen(key)==strlen("apply.output")){
		return 1;
	}else{
		return 0;
	}
}

char*
searcher(char *path,char *dir_command,char *argv[])
{

	DIR *d;
	int ok;
	int found_txt;
	int pid, fd_1;


	ok = stat(path, &s);
	if (ok < 0){
		exit(EXIT_FAILURE);
	}
	if((s.st_mode & S_IFMT) == S_IFDIR){
		d = opendir(path);
		if (d == NULL)
			exit(EXIT_FAILURE);
		unlink("apply.output");
		for(;;)
		{
			reg_dir = readdir(d);
			if(reg_dir==NULL)
				break;
			if (reg_dir->d_name[0] == '.'){
				continue;
			}else{
				memset(path_new, 0, 2048);
				strcat(path_new, path);
				strcat(path_new, "/");
				strcat(path_new, reg_dir->d_name);

				ok=stat(path_new, &s);
				if(ok<0)
					exit(EXIT_FAILURE);
				if((s.st_mode & S_IFMT) == S_IFREG){
					found_txt = search_txt(path_new);
					if (found_txt){
						pid = fork();
						switch(pid){
							case -1:
								err(1, "fork failed");
								break;
							case 0:

								// abro el .txt y lo pongo como entrada estandar
								open_file_in(path_new);
								fd_1 = open("apply.output",O_APPEND|O_CREAT|O_WRONLY, 0660);
								if (fd_1 < 0) {
									err(1, "afile");
								}
								dup2(fd_1,1);
								close(fd_1);
								execv(dir_command, argv);
								perror(dir_command);
								exit(EXIT_FAILURE);

							default:
								wait(NULL);
					}


				}
			}

		}
	}

	}
	return path_new;
}


int
main(int argc, char *argv[])
{

	char *path;
	char *command;
	char *dir_command;

	command = argv[1];
	argv++;
	dir_command = ok_command(command);
	path = getcwd(buffer, sizeof buffer);
	searcher(path,dir_command,argv); // aqui me guardo el ".txt" que me voy encontrando




	exit(EXIT_SUCCESS);
}
