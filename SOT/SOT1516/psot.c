
// Jhon Fajardo Rodas Tecnologias
// Practica Final


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

typedef struct line_command line_command;
struct line_command{
	char *command;
	char *arguments[64];
};

line_command array_commands[128];
char *files_tst[1024];
char *files_ok[1024];
char *files_out[1024];
char *array_aux[1024];
int n_ok = 0;
char path_aux[2048];
char *array_paths[1024];
int array_pid[128];
char *vePath;
int n_paths;
int pos_pipes[50][2];


int
tokenize(char *str, char *limit, char *array[]){

char *token, *saveptr;
int n = 0;

	token = strtok_r(str, limit, &saveptr);
	while(token){

		array[n] = strdup(token);
		n++;
		token = strtok_r(NULL, limit, &saveptr);
	}
	return n;
}

void
creat_pipes(int n_commands){
	int i;
	int n_pipes = n_commands-1;
	for(i = 0; i < n_pipes; i++){
		if (pipe(pos_pipes[i]) == -1){

			perror("pipe");
			exit(EXIT_FAILURE);
		}
	}
}

void
close_pipes(int n_commands){
	int j;
	int n_pipes = n_commands -1;
	for(j = 0; j < n_pipes; j++){
		close(pos_pipes[j][1]);
		close(pos_pipes[j][0]);
	}
}

void
change_in_std(){
	int fd;
	fd = open("/dev/null", O_RDONLY);
	if (fd < 0){
		err(1, "open file: /dev/null");
	}
	dup2(fd, 0);
	close(fd);	
}

void
change_err_std(char *file){
	int fd;
	fd = open(file,O_WRONLY | O_APPEND);
	if (fd < 0){
		err(1, "creat file: .out");
	}
	dup2(fd, 2);
	close(fd);	
}

void
change_out_std(char *file){
	int fd;
	fd = creat(file,0660);
	if (fd < 0){
		err(1, "open file: .out");
	}
	dup2(fd, 1);
	close(fd);	
}


int
ok_tst(char *path)
{
	int ok = 0;

	char *key;
	key = strstr(path, ".tst");
	if ((key != NULL) && strlen(key) == strlen(".tst")){
		ok = 1;
	}

	return ok;
}

int
ok_ok(char *path)
{
	int ok = 0;

	char *key;
	key = strstr(path, ".ok");
	if ((key != NULL) && strlen(key) == strlen(".ok")){
		ok = 1;
	}

	return ok;
}

char*
creat_file_out(char *file)
{

	char *fich_out;

	int n_out;
	fich_out = malloc(2048);
	n_out = tokenize(file, ".", array_aux);
	if (n_out > 1){
		
		strcat(fich_out, array_aux[0]);
		strcat(fich_out, ".out");
	}

	return fich_out;
	
}


char *
creat_name_ok(char *file)
{
	char *fich_ok;
	fich_ok = malloc(2048);

	strcat(fich_ok, file);
	strcat(fich_ok, ".ok");
	return fich_ok;
}


int
creat_struct_command(char *str, int n_command, char*cwd)
{
	char *limit = " \t\n";
	int n_arguments;
	char *aux[1024];
	int change_dir = 0;
	int n, n_new;
	char path_ch[1024];
	int i;

	n_arguments = tokenize(str, limit, array_commands[n_command].arguments);

	if(strcmp(array_commands[n_command].arguments[0], "cd") == 0)
	{
		
		
		if(n_arguments == 1)
		{
			
			n = chdir(getenv("HOME"));
			if (n < 0){
				fprintf(stderr, "%s\n", "Error: change to home");
			}
		}else{
			memset(path_ch, 0, 1024);
			strcat(path_ch, cwd);
			strcat(path_ch, "/");
			strcat(path_ch, array_commands[n_command].arguments[1]);
			
		
			n = chdir(path_ch);

			if (n < 0){
				strcat(path_ch, ".dir");
				
				n_new  =  chdir(path_ch);
				if (n_new < 0){
					fprintf(stderr, "%s\n", "Error: change to dir");
				}
				
			}
		}
	}
	
	
	for(i = 0; i < n_arguments; i ++){
		
		if (array_commands[n_command].arguments[i][0] == '$'){
			tokenize(array_commands[n_command].arguments[i], "$", aux);
			array_commands[n_command].arguments[i] = getenv(aux[0]);
			
			if(array_commands[n_command].arguments[i] == NULL){
				fprintf(stderr, "error: var %s does not exist \n", aux[0]);
				continue;
			}
			
		}
	}

	array_commands[n_command].arguments[n_arguments] = NULL;
	array_commands[n_command].command = array_commands[n_command].arguments[0];

	return change_dir;
}

void
tok_Path(int nAux)
{

	int j;
	int n;
	char path_aux_paths[2048];
	for (j = 0; j < nAux; j++)
	{

		n = strlen(array_paths[j]);
		memset(path_aux_paths, 0, 2048);
		strcat(path_aux_paths, array_paths[j]);
		if(array_paths[j][n-1] != '/')
			strcat(path_aux_paths, "/");

		array_paths[j] = strdup(path_aux_paths);
	}
}

char*
cat_command(char *command, char *dir){

	memset(path_aux, 0, 2048);
	strcat(path_aux, dir);
	strcat(path_aux, command);
	return path_aux;
}

char*
ok_command(char *command)
{

	int ok = 0;
	char *path_command = NULL;

	int i = 0;
	while((!ok) && (i < n_paths))
	{
		if(access(cat_command(command, array_paths[i]), X_OK) == 0)
		{
			path_command = cat_command(command, array_paths[i]);
			ok = 1;
		}else{
			i++;
		}
	}

	return path_command;

}



int
searcher(char *path)
{
	int ok_dir;
	DIR *d;
	ok_dir = stat(path, &s);
	char path_new[2048];
	int n = 0;
	if(ok_dir < 0)
		exit(EXIT_FAILURE);

	if((s.st_mode & S_IFMT) == S_IFDIR) // compruebo si es un directorio
	{

		d = opendir(path);
		if (d == NULL)
			exit(EXIT_FAILURE);
		for(;;)
		{
			reg_dir=readdir(d);
			if (reg_dir == NULL)
				break;

			if (reg_dir->d_name[0] == '.')
			{ // paso de los directorios raiz
				continue;
			}else{
				memset(path_new, 0, 2048);
				strcat(path_new, path);
				strcat(path_new, "/");
				strcat(path_new, reg_dir->d_name);

				ok_dir=stat(path_new, &s);

				if(ok_dir<0)
					exit(EXIT_FAILURE);
				if((s.st_mode & S_IFMT) == S_IFREG)
				{ // si es un fichero compruebo si es un txt
					if (ok_tst(path_new))
					{
						files_tst[n] = strdup(path_new);
						n++;

					}else if(ok_ok(path_new))
					{
						files_ok[n_ok] = strdup(path_new);
						n_ok++;
					}

				}
			}


		}
	}

	return n;
}


void
creat_fork(int n_com, char *file_o)
{
	int i;
	char *path_command = NULL;
	if (n_com > 1){
		creat_pipes(n_com);
	}

	for(i=0; i < n_com ; i++)
	{
		array_pid[i] = fork();
		switch(array_pid[i])
		{
			case -1:
				err(1, "fork failed");
				break;
			case 0:
				if(i == 0)
				{
					
					change_in_std();
				}
				if(i == n_com -1){
						
						change_out_std(file_o);
				}

				if(n_com > 1)
				{
					if(i==0){
						
						dup2(pos_pipes[0][1], 1);
					}else if(i == n_com-1){
						
						dup2(pos_pipes[i-1][0], 0);
					
					}else{
						
						dup2(pos_pipes[i-1][0], 0);
						dup2(pos_pipes[i][1], 1);
					}
					close_pipes( n_com);
				}
				
				path_command = ok_command(array_commands[i].command);

				execv(path_command, array_commands[i].arguments);
				exit(EXIT_FAILURE);
		}

	}
	if(n_com > 1){
		close_pipes(n_com);
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
compare_file(char *f1, char *f2)
{
	FILE *f_aux1, *f_aux2;
	char line1[512];
	char line2[512];
	int eq=1;
			
	f_aux1 = fopen(f1, "r");
	f_aux2 = fopen(f2, "r");
	if ((f_aux1 < 0) || (f_aux2 < 0))
	{
		fprintf(stderr, "%s\n", "Error: error in open files.ok or files.out");
		exit(EXIT_FAILURE);
	}
	
	while((fgets(line1, 100, f_aux1) != NULL ) && ( fgets(line2, 100, f_aux2)!= NULL) && (eq!=0))
	{
		
		if(strcmp(line1, line2) != 0){
			eq = 0;
		}

	}

	fclose(f_aux1);
	fclose(f_aux2);

	return eq;

}


char*
reader_file(char *path_file, char *cwd){
	FILE *file;
	char line[512];
	int n_total_commads = 0;
	int change_dir;
	
	
	file = fopen(path_file, "r");
	if (file == NULL)
	{
		fprintf(stderr, "%s\n", "Error: error in open file.tst");
		exit(EXIT_FAILURE);
	}

	
	while(fgets(line, 100, file)!= NULL)
	{
		if(strcmp(line, "\n") == 0)
			continue;
		
		change_dir = creat_struct_command(line, n_total_commads, cwd);
		if (change_dir == 0)
			n_total_commads++;
	}

	char *f_o;
	f_o =  creat_file_out(path_file);
	creat_fork(n_total_commads, f_o);
	
	close_fork(n_total_commads);

	if(change_dir == 1)
		chdir(cwd);


	return f_o;

}

void
create_file_ok(char *f_ok, char *f_out)
{
	FILE *fd, *fd_o;
	char line[512];

	fd = fopen(f_ok, "w");
	if (fd == NULL)
	{
		fprintf(stderr, "%s\n", "Error: error in creat file.ok");
		exit(EXIT_FAILURE);
	}
	fd_o = fopen(f_out, "r");
	if (fd_o == NULL)
	{
		fprintf(stderr, "%s\n", "Error: error in creat file.out");
		exit(EXIT_FAILURE);
	}

	while(fgets(line, 100, fd_o) != NULL)
	{
		fputs(line, fd);
	}

	fclose(fd);
	fclose(fd_o);
}

int
main (int argc, char *argv[])
{

	char *cwd;
	char path[1024];
	int n_tst;
	char *f_ok, *f_out;
	int j;
	int i;
	int found;
	int array_status[50];

	if(argc > 1)
	{
		fprintf(stderr, "%s\n", "Error: usage error in number of arguments");
		exit(EXIT_FAILURE);
	}
		
	cwd = getcwd(path, 64);
	n_tst = searcher(cwd);

	vePath = getenv("PATH");
	n_paths = tokenize(vePath, ":", array_paths);
	tok_Path(n_paths);

	
	for (i = 0; i < n_tst; i++)
	{

		f_out = reader_file(files_tst[i], cwd);
		change_err_std(f_out);
		f_ok = creat_name_ok(files_tst[i]);
		found = 0;
		for(j = 0; j < n_ok; j++)
		{
			if(strcmp(f_ok, files_ok[j]) == 0){
				array_status[j] = compare_file(f_ok, f_out);
				found = 1;
				break;
			}
		}

		if (found == 0){
			create_file_ok(f_ok, f_out);
		}
			
	}
	int found_e = 0;
	int p;
	for(p = 0; p < n_ok; p++){
		if(array_status[p] == 0)
		{
			found_e = 1;
			break;
		}
	}
	
	if((n_ok == 0) || (!found_e)){
		printf("%s\n", "Los test han sido superados.");
		exit(EXIT_SUCCESS);

	}else{
		printf("%s\n", "Los test no han sido superados");
		exit(EXIT_FAILURE);
	}
	

	

}
