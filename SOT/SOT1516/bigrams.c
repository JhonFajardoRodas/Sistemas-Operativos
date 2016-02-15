/* Ejercicio: 8
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
#include <fcntl.h>
#include <sys/mman.h>


int matrix[128][128];


void
fill_matrix(void)
{
	int i, j;
	for(i=0; i < 128; i++)
	{
		for(j=0; j < 128; j++)
		{
			matrix[i][j] = 0;
		}
	}
}

void
print_matrix(void)
{
	int i, j;
	for(i=0; i < 128; i++)
	{
		for(j=0; j < 128; j++)
		{
			if(matrix[i][j] != 0)
				printf("%d ", matrix[i][j]);
		}
	}

}	

int 
main(int argc, char *argv[])
{

	FILE *file;
	char c, next_c;
	int x, y, countBi;
	
	int fd;
	fill_matrix();

	fd = creat(argv[1], 0644);
	if(fd < 0) {
		err(1, "create");
	}
	lseek(fd, 128*128, 0);
	int z;
	for(z = 0; z < 128*128; z++)
	{
		if(write(fd, "0", z) != 1) {
		err(1, "write");
	}
	}
	


	file = fopen(argv[2], "r");
	
	if(file == NULL)
	{
		fprintf(stderr, "%s\n", "Error: open file");
		exit(EXIT_FAILURE);
	}
	
	c = fgetc(file);

	while(!feof(file))
	{	
		next_c = fgetc(file);
		x = c;
		y = next_c;
		countBi = matrix[x][y];
		if(countBi >= 255){
			countBi = 255;
		}else{
			countBi++;
		}	
		matrix[x][y] = countBi;
		c = next_c;
	}
	fclose(file);

	print_matrix();
	printf("\n");

	exit(EXIT_SUCCESS);
}