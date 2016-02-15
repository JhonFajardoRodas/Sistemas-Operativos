/* EJERCICIO: 3
  Jhon Fajardo Rodas TEcnologías
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

typedef struct Caracter Caracter ;
struct Caracter {
	char letra;
	int count;
	float porcentaje;
};


typedef struct ListChar ListChar;
struct ListChar
{	
	Caracter c;
	ListChar *next;
	
};

ListChar *lista;
int n_an = 0;  // numero de alfanumericos
int i_ok = 0;


// Funcion para buscar el numero de caracteres alfanumericos en el fichero

void
print_list()
{

	ListChar *aux2;
	aux2 = lista;
	int p = 0;

	while (aux2 != NULL){
		
		printf("%c %.2f%c %d %d\n", aux2->c.letra, aux2->c.porcentaje, '%', p, p);
		aux2 = aux2->next;

	}
}


int
buscar(char c)
{
	ListChar *aux;
	aux = lista;
	int found = 0;
	int n;


	while((aux != NULL) && (found == 0)){

		if (aux->c.letra == c){
			n = aux->c.count;
			found = 1;
			n++;
			aux->c.count = n;
		}
		aux = aux->next;
	}

	return found;
}


void 
num_AlfaNum (int nr, char *str)
{
	
	int i = 0;
	char m;
	
	ListChar *new_aux;
	

	for (i = 0; i < nr ; i++){
		m = str[i];
		if(!i_ok)
			m = tolower(str[i]);
		
		//printf("-------->%c<-------\n", str[i]);
		if(isalnum(m)){
			
			if (!buscar(m)){ // mira si no esta vacia la lista-- meterlo en un bucle
				  
				//printf("[%c]\n", str[i]);
				new_aux = malloc(sizeof(ListChar));
				new_aux->c.letra = str[i];	
				new_aux->c.count = 1;	
				new_aux->c.porcentaje = 0.00;
				new_aux->next = lista;
				lista= new_aux;
				
			}
			
			n_an++;	
			
		}	
		
	}

	
}

void 
porcnt (void)
{

	float por;
	int count;
	ListChar *aux;
	aux = lista;
	while(aux != NULL){
		por = aux->c.porcentaje;
		count = aux->c.count;
		por = (count*100.00)/n_an;
		aux->c.porcentaje = por;

		aux=aux->next;

	}

}


int
main (int argc, char *argv[])
{

	int fd;
	char *fich;
	
	char buffer[1024];
	int nr;
	char *str;
	

	if (argc == 2){
		fich = argv[1];
		fd = open(fich, O_RDONLY);
		//
		if (fd < 0){
				err(1,"open file");
		}

		nr = read(fd, buffer, sizeof buffer);

		if(nr < 0){
			err(1, "read");
		}
		close(fd);
		
		num_AlfaNum(nr, buffer);
		
		porcnt();
		print_list();	

	}else if ((argc == 3) && (strcmp(argv[1] ,"-i")==0)){
		fich = argv[2];
		i_ok = 1;
		fd = open(fich, O_RDONLY);
		if (fd < 0){
			err(1,"open file");
		}

		nr = read(fd, buffer, sizeof buffer);

		if(nr < 0){
			err(1, "read");
		}
		close(fd);
		
		num_AlfaNum(nr, buffer);
		
		porcnt();
		print_list();	
	}else if(argc == 1){
		//printf("hay - w \n");
		str = fgets(buffer, 1024, stdin);
		nr = strlen(buffer);
		if(str == NULL){
			fprintf(stderr, "%s\n", "Usage error");
		}
		
		num_AlfaNum(nr, buffer);

		porcnt();
		print_list();

	}else if ((argc > 2) && (strcmp(argv[1] ,"-i") !=0)){
		
		int i;
		for (i = 1; i < argc; i++){
			fich = argv[i];
			fd = open(fich, O_RDONLY);
			if (fd < 0){
				err(1,"open file");
			}

			nr = read(fd, buffer, sizeof buffer);

			if(nr < 0){
				err(1, "read");
			}
			close(fd);
			
			
			num_AlfaNum(nr, buffer);

			porcnt();
			print_list();
			printf("\n");

		}

	}
	
	exit(EXIT_SUCCESS);
}
