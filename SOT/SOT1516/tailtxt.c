/* Ejercicio: 4
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


int
ok_txt(char *path)
{
	int ok = 0;

	char *key;
	key = strstr(path, ".txt");
	if ((key != NULL) && strlen(key) == strlen(".txt")){
		ok = 1;
	}



	return ok;
}


int
searcher(char *path, int n_bytes)
{

	DIR *d;
	int ok;
	int n=0;
	char path_new[2048];

	char buffer[1024];
	//char* buffer  = (char*)malloc(1024);
	int fd, nr;

	ok = stat(path, &s); // busco el status del path
	if (ok < 0){
		exit(EXIT_FAILURE);
	}
	if((s.st_mode & S_IFMT) == S_IFDIR){ // compruebo si es un directorio
		d = opendir(path);
		if(d == NULL)
			exit(EXIT_FAILURE);
		for(;;)
		{
			reg_dir=readdir(d);
			if (reg_dir == NULL)
				break;
			if (reg_dir->d_name[0] == '.'){ // paso de los directorios raiz
				continue;
			}else{
				memset(path_new, 0, 2048);
				strcat(path_new, path);
				strcat(path_new, "/");
				strcat(path_new, reg_dir->d_name);

				ok=stat(path_new, &s); // compruebo el path construido
				if(ok<0)
					exit(EXIT_FAILURE);
				if((s.st_mode & S_IFMT) == S_IFDIR){ // si me encuentro otro directorio lo hago recursivo
					n = n + searcher(path_new, n_bytes);

				}else if((s.st_mode & S_IFMT) == S_IFREG){ // si es un fichero compruebo si es un txt
					if(ok_txt(path_new)){ // aqui miro para imprimir los bytes

						fd = open(path_new, O_RDONLY);
						if (fd < 0){
							fprintf(stderr, "%s %s\n", "Fallo al abrir el fichero:   ", path_new);
						}

						if (n_bytes != 0)
							lseek(fd, -n_bytes, SEEK_END);
						nr = read(fd, buffer, sizeof buffer);
						if(nr < 0){
							fprintf(stderr, "%s %s\n", "Fallo al leer el fichero:   ", path_new);
						}

						write(1, buffer, nr);
						printf("\n");
						close(fd);

						n++;


					}
				}
			}
		}
	}


	return n;
}


int
main (int argc, char *argv[])
{

	char *cwd;
	char path[1024];
	int n_bytes = 0;

	if (argc > 1)
		n_bytes = atoi(argv[1]);
 	cwd = getcwd(path, 64);
	searcher(cwd, n_bytes);

	exit(EXIT_SUCCESS);
}
