// Jhon Fajardo Rodas
// TEcnologías
// Práctica Final: sh.c
// gcc -g -o sh -Wall -Wshadow sh.c

 
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

typedef struct line_command line_command;
struct line_command{
	char *command;
	char *arguments[64];
};

line_command array_commands[128];
char path_aux[2048];
int n_pipes, n_ve;
char *array_file[1024];
char *array_ve[1024];
char *array_ve2[1024];
char *aux[1024];
int array_pid[128];
char *array_b[1024];
char *file_i;
char *file_o;
char *array_aux[1024];
int no_ve;

int 
tokenize(char *str, char *limit, char *array[]){

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
creat_pipes(int pos[50][2]){
	int i;

	for(i = 0; i < n_pipes; i++){
		if (pipe(pos[i]) == -1){

			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
}

void
close_pipes(int pos[50][2]){
	int j;
	for(j = 0; j < n_pipes; j++){
		close(pos[j][1]);
		close(pos[j][0]);
	}
}



void
creat_ve (char *str, char *limit){

	char *ve1, *ve2;
	n_ve = tokenize(str, limit, array_ve);
	if(n_ve == 2){
		ve1 = array_ve[0];
		ve2 = array_ve[1];

		n_ve = tokenize(ve1, " \n", array_ve);
		ve1 = array_ve[0];
		n_ve = tokenize(ve2, " \n", array_ve2);
		ve2 = array_ve2[0];
		setenv(ve1, ve2, 1);
	}
}

void
eject_ve(char *str[],int n){
	int i;

	for(i = 0; i < n; i ++){
		
		if (str[i][0] == '$'){
			tokenize(str[i], "$", aux);
			str[i] = getenv(aux[0]);
			if(str[i] == NULL){
				fprintf(stderr, "error: var %s does not exist \n", aux[0]);
				no_ve = 1;
				continue;
			}
			
			if (array_commands[i].arguments == NULL){
				fprintf(stderr, "Error \n");
			}
		}
	}
}

int
change_dir(void){
	char *home;
	int chg;
	int change = 0;
	
	if(strcmp(array_commands[0].arguments[0], "cd") == 0){
		if(array_commands[0].arguments[1] == NULL){
			home = getenv("HOME");
			chg = chdir(home);
			change = 1;
			if(chg < 0){
				fprintf(stderr, "error of change dir\n");
			}	
		}else{
			chg = chdir(array_commands[0].arguments[1]);
			change = 1;
			if(chg < 0){
				fprintf(stderr, "error of change dir\n");
			}
		}
	}
	return change;
}


char*
redirects(char *str){
	int n, n1, n2;
	char *rest;
	n  = tokenize(str, ">\n", array_file);
	if(n > 1){
		n1 = tokenize(array_file[1], "<\n", aux);
		if(n1 > 1){	
			rest = array_file[0];
			file_i = aux[1];
			file_o = aux[0];
			tokenize(file_o, " \t", aux);
			file_o = aux[0];
			if(file_o[0] == '$'){
				tokenize(file_o, "$", aux);
				file_o = getenv(aux[0]);
			}
			tokenize(file_i, " \t", aux);
			file_i = aux[0];
			if(file_i[0] == '$'){
				tokenize(file_i, "$", aux);
				file_i = getenv(aux[0]);
			}
		
			
		} else{
			
			file_o = array_file[1];
			tokenize(file_o, " \t", aux);
			file_o = aux[0];
			if(file_o[0] == '$'){
				tokenize(file_o, "$", aux);
				file_o = getenv(aux[0]);
			}
			

			n2 = tokenize(array_file[0], "<\n", aux);
			if(n2 > 1){
				file_i = aux[1];
				rest = aux[0];
				tokenize(file_i, " \t", aux);
				file_i = aux[0];
				if(file_i[0] == '$'){
					tokenize(file_i, "$", aux);
					file_i = getenv(aux[0]);
				}
				
		
			}
			rest = array_file[0];
		}
	}else{
		n1 = tokenize(str, "<\n", array_file);
		if(n1 > 1){
			file_i = array_file[1];

			rest = array_file[0];
			tokenize(file_i, " \t", aux);
			file_i = aux[0];
			if(file_i[0] == '$'){
				tokenize(file_i, "$", aux);
				file_i = getenv(aux[0]);
			}
			
		}else{
			rest = str;
		}

	}
	//
	return rest;
}

void
open_file_in(char *file){
	int fd;
	fd = open(file, O_RDONLY);
	if (fd < 0){
		err(1, "open file");
	}
	dup2(fd, 0);
	close(fd);	
}

void
open_file_out(char *file){
	int fd;
	fd = creat(file,0660);
	if (fd < 0){
		err(1, "open file");
	}
	dup2(fd, 1);
	close(fd);	
}

char*
cat_command(char *command, char *dir){
	
	memset(path_aux, 0, 2048);
	strcat(path_aux, dir);
	strcat(path_aux, command);	
	return path_aux;
}

char*
ok_command(char *command){
	char dir_x[] = "./";
	char dir_1[] = "/bin/";
	char dir_2[] = "/usr/bin/";
	char dir_3[] = "/usr/local/bin/";
	char *path_p = NULL;
	
	if (access(cat_command(command, dir_x), X_OK)==0){
		path_p = cat_command(command, dir_x);
	}else if (access(cat_command(command, dir_1), X_OK)==0){
		path_p = cat_command(command, dir_1);
	}else if(access(cat_command(command, dir_2), X_OK)==0){
		path_p = cat_command(command, dir_2);	
	}else if (access(cat_command(command, dir_3), X_OK)==0){
		path_p =  cat_command(command, dir_3);	
	}

	
	
	return path_p;
}

void
creat_fork(int n_commands, int pos[50][2],  int back_ok){
	
	int i, fd;
	char *path_command = NULL;
	
	for (i = 0; i < n_commands; i++){

		array_pid[i] = fork(); 
		switch(array_pid[i]){
			case -1:
				err(1, "fork failed");
				break;
			case 0:
				if(n_commands == 1){
					if(file_i != NULL)
						open_file_in(file_i);
					if(file_o != NULL)
						open_file_out(file_o);
				}
				
				if ((i==0) && (file_i==NULL) && back_ok){
					fd=open("/dev/null",O_RDONLY);
					if(fd<0)
						err(1,"open");
					dup2(fd,0);
					close(fd);
					
				}
				if(n_commands > 1){
					if(file_i != NULL)
						open_file_in(file_i);
					if(file_o != NULL)
						open_file_out(file_o);
					if(i == 0){
						
						dup2(pos[0][1], 1);
					}else if(i == n_commands -1){
						dup2(pos[i-1][0], 0);
						
					}else{
						dup2(pos[i-1][0], 0);
						dup2(pos[i][1],1);
					}
					close_pipes(pos);
				}
				path_command = ok_command(array_commands[i].command);
				execv(path_command, array_commands[i].arguments);
				exit(EXIT_FAILURE);
		}
	}
}

int 
found_pid(int n_commands, int pid){
	int i;
	int found = 0;
	for(i = 0; i <= n_commands; i++){
		if (pid == array_pid[i]){
			found = 1;
			
		}
	}
	return found;
}

void 
close_fork(int n_commands){
	int pid;
	int found = 0;
	int pid_ok = 0;
	for (;;){
		pid = wait(NULL);
		found = found_pid(n_commands, pid);
		if(found){
			pid_ok++;
		}
		if(pid_ok == n_commands){
			break;
		}
	}
}

int
found_back(char *str){
	int back = 0;
	int n_back;
	
	n_back = tokenize(str, "&", array_b);
	if(n_back > 1){
		back = 1;
	}

	return back;

}

int
main(){

char buffer[1024];
char *str;
int n_commands, n_arguments;	
char *limit = "|\n";
e
int i, change;
int pos_pipe[50][2];
int back_ok = 0;
char *rest_buffer;

 

	for(;;){
		printf("jhon-shell:~$ ");
		str = fgets(buffer, 1024, stdin);		
		if (str ==  NULL){
			break;
		}
		file_i = NULL;
		file_o = NULL;
		if((buffer[0] == '\n')||(buffer[0] == ' ')){
			continue;
		}
		
		back_ok = found_back(buffer);
		
		if(back_ok){
			strcpy(buffer, array_b[0]);
		}
		no_ve = 0;
		creat_ve(buffer, "=");  //---------- aqui saco las VE	

		
		rest_buffer = redirects(buffer);
		// SACO EL NUMERO DE COMANDOS 
		
		
		n_commands = tokenize(rest_buffer, limit, array);
			
		// ALMACENO EN MI ESTRUCTURA
		for(i = 0; i < n_commands; i++){				
			n_arguments = tokenize(array[i]," ", array_commands[i].arguments);
			eject_ve(array_commands[i].arguments, n_arguments);

			array_commands[i].arguments[n_arguments] = NULL;
			array_commands[i].command = array_commands[i].arguments[0];	
		}
		// CAMBIO DE DIRECTORIO
		if (no_ve == 0){
			change = change_dir();
		}
		
			

		if(change  ==  1){
			continue;
		}else{
			n_pipes = n_commands-1;
			if(n_commands == 1){
				creat_fork(n_commands, pos_pipe, back_ok);	
			}
			if(n_commands > 1){
				creat_pipes(pos_pipe);
				creat_fork(n_commands, pos_pipe, back_ok);	
			}
			close_pipes(pos_pipe);
			// si encuentro el & no lo hago
			
			if(back_ok == 0){
				close_fork(n_commands);	
			}
						
		}
		
			
	}

	exit(EXIT_SUCCESS);
}
