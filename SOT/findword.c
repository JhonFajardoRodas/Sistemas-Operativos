/* EJERCICIO: 5
 * 
 * Jhon Fajardo Rodas  Tecnologías*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/types.h>       
	#include <unistd.h>
	#include <err.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <dirent.h>
	
	struct dirent *reg_dir;
	struct stat s;
	

	
	
	void 
	search_file(char *key_word, char *file)
	{
		int fd;
		int nr;
		char *buffer;
		buffer = malloc(strlen(key_word)*sizeof(char));
		fd = open(file, O_RDONLY);
		if (fd < 0)
			return;
		nr = read(fd, buffer, strlen(key_word))	;
		if(nr < 0 || nr == 0){
			return;
		}else{
			if(strcmp(key_word, buffer) == 0)
				printf("%s\n", file);
			
		}
			
		
	}
	void
	list_reg(char *key_word, char *path)
	{
		
		DIR *d;
		int ok = stat(path, &s);
		char path_new[2048];
		
		if(ok < 0)
			return;
		if(s.st_mode & S_IFDIR){
			d = opendir(path);
			if (d == NULL)
				exit(EXIT_FAILURE); 
			for(;;)
			{
					reg_dir = readdir(d);
					if(reg_dir == NULL)
						break;
					if(strcmp(reg_dir->d_name, ".")	== 0 || strcmp(reg_dir->d_name, "..") == 0){
						continue;
					}else{
						
						memset(path_new,0,2048);
						
						
						strcat(path_new, path);
						strcat(path_new,"/");
						strcat(path_new, reg_dir->d_name);
				
						// hago el nuevo stat con el path nuevo
						ok = stat(path_new, &s);
						if (ok<0)
							return;
						if(s.st_mode & S_IFDIR){
							list_reg(key_word, path_new);
						}else{
							search_file(key_word, path_new);
						}
					}		
			}
		
		}else{
			search_file(key_word,path);
			
		}
			 
	}
	
	int
	main (int argc, char *argv[])
	{
		
		int i;
		
		if (argc == 1){
			fprintf(stderr, "USAGE ERROR\n");
		}else if(argc == 2){
			list_reg(argv[1], ".");
		}else if(argc > 2){
			for(i = 2; i < argc; i++){
				list_reg(argv[1], argv[i]);
			}
		}
		
		exit(EXIT_SUCCESS);
	}
	
	
