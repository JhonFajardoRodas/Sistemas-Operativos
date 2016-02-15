/* EJERCICIO: 6
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
	#include <sys/wait.h>
	struct dirent *reg_dir;
	struct stat s;

int
rmrubbish(char *path)
{

	char *key;
	int delete;
	key = strstr(path, ".rubbish");
	if((key != NULL) && strlen(key)==strlen(".rubbish")){
		delete = unlink(path);
		if (delete == 0){
				return 0;
				
		}else{
			fprintf(stderr,"%d: error removing dir %s\n",getpid(),path);
			exit(EXIT_FAILURE);
		}
		

	}else{
		return 1;
	}

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
	if((s.st_mode & S_IFMT) == S_IFDIR){
		d = opendir(path);
		if(d == NULL)
			exit(EXIT_FAILURE);
		for(;;)
		{
			reg_dir=readdir(d);
			if (reg_dir == NULL)
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
				if((s.st_mode & S_IFMT) == S_IFDIR){
					n = n + searcher(path_new);
				}else if((s.st_mode & S_IFMT) == S_IFREG){
					if(rmrubbish(path_new)== 0){
						n++;
					}
				}
			}
		}

		
	}

	return n;
} 

 int
 main(int argc, char *argv[])
 {

 	int  j,i, pid, sts;
 	int n = 0;
 	int bad = 0;
 	
 	if (argc == 1){
 		fprintf(stderr, "USAGE ERROR\n");
 	}else if(argc > 1){
 		for(i = 1; i < argc; i++){
 			
 			pid = fork();
 			switch(pid)
 			{
 				case -1: 
 					err(1, "fork failed");
					break;
				case 0: 
					n = searcher(argv[i]);
					if(n == 0){
						fprintf(stderr, "%d : no files to remove in dir %s\n", getpid(), argv[i]);
					}else{
						fprintf(stderr, "%d : %s ok\n", getpid(), argv[i]);
					}
					exit(n==0);
 			}
 		}
 		for (j = 1; j < argc; j++)
 		{
 	
 			if (wait(&sts) < 0){
 				break;
 			}else if(sts != 0){
 				bad ++;

 			}else{

 			}
 		}
 			
 		
 		if (bad == 0){
			fprintf(stderr, "all processes were successful\n");
		}else{
			fprintf(stderr, "%d  processes failed\n", bad);	
		}
 	}

 	


 	exit(EXIT_SUCCESS);
 }
