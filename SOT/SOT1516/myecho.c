/* EJERCICIO: 2

 * 
 * Jhon Fajardo Rodas  Tecnologías*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
 check_n(int long_array, char *str){
 	int found = 0;
 	char *limitador = "-n";
 	int n;
	n = strcmp(limitador, str);
	
	if (n==0)
		found = 1;

	return found;
 }

 int
 check_pid(int long_array, char *str[]){
 	char *limitador = "*";
 	
 	int n;
 	
 	if(strcmp(limitador, str[1]) == 0){
 		//printf("ENCUENTRO ASTERISCO--\n");
 		n = getpid();
 		
 	}else{
 		n = 0;
 	}
 	
 	return n;
 	
 }

 int
 main(int argc, char *argv[]){

 	int i;
 	int n_pid;
 	char *cwd;
 	char path[512];
 	char aux[64];

 	

	n_pid = check_pid(argc, argv);
	if(n_pid != 0){
		printf("%d\n", n_pid);
	}else{
		for (i = 1; i < argc; i++){
			if (strcmp(argv[i], "CASA") == 0){
				strcpy(aux, getenv("HOME"));
				printf("%s ", aux);

			}else if (strcmp(argv[i], "DIRECTORIO") == 0){
				cwd = getcwd(path, 64);
	 			if (cwd != NULL)
	 				strcpy(aux, path);
	 				printf("%s ", aux);

			}else if (strcmp(argv[i], "USUARIO") == 0){
				strcpy(aux, getenv("USER"));
				printf("%s ", aux);

			}else{
				if(check_n(argc, argv[i]) == 0){
					printf("%s ", argv[i]);

				}
				
			}

		}

		if (!check_n(argc, argv[1]))
				printf("\n");

	}

	

 	exit(EXIT_SUCCESS);
 }