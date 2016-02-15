/* EJERCICIO: 4
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

	typedef struct Coord Coord;
	struct Coord
	{
		int x;
		int y;
	};
	

	
	
	typedef struct TipoLista TipoLista;
	struct TipoLista
	{
		Coord c;
		TipoLista *next;
	};
	
	
	void
	printList(TipoLista **lista, int tam_lista)
	{
		TipoLista *auxiliar;
		int i;
		
		for (i=0; i<tam_lista; i++){
			auxiliar = *lista;
			printf("(%d,%d)\n", auxiliar->c.x, auxiliar->c.y);
			*lista = (*lista)->next;
			free(auxiliar);
		}
		
	}
	
	
	void 
	insertar(Coord coordenada, TipoLista **lista)
	{
	

	TipoLista *nuevo;
	

	nuevo = malloc(sizeof(TipoLista));
	nuevo->c = coordenada;
	nuevo->next = *lista;
	
	
	*lista = nuevo;
}


	int
	opt_buffer(int nr, int size_coor)
	{
		int rest;
		rest = nr%size_coor;
		return rest;
	}
	


	int 
	main(int argc, char *argv[])
	{
		
		int fd;
		char buffer[1024];
		int nr; 
		Coord* bufferCoor;
		int n_coords;
		int j;
		TipoLista *listaCoor = NULL;
		int tam_lista = 0;
		int rest;

		
		if (argc==3){
			if(strcmp(argv[1],"-w") == 0){
				int i;
				for (i = 0; i <= atoi(argv[2]); i++){

					write(1, &i, 4);
					write(1, &i, 4);
			
					}
			}
		}
		
		if ((argc==1) || (argc==2)){
			if (argc==1){
				fd=0;
			}
			if (argc == 2){
				fd = open(argv[1], O_RDONLY);
				//
				if (fd < 0){
					err(1,"open file");
				}
					
			}
			for(;;)
			{
				
				
				nr = read(fd, buffer, sizeof buffer);
				rest = opt_buffer(nr, sizeof(Coord));
		
				if(nr == 0){
					break;
				}
				if(nr < 0){
					err(1, "read");
				}
				bufferCoor = (Coord*)buffer;
				n_coords = nr/sizeof(Coord);
				if (rest == 0){
					for(j = 0; j < n_coords; j++){			
						insertar(bufferCoor[j], &listaCoor);
						tam_lista++;
					}
				
						
				
						
				}
				
			
			
			}
			printList(&listaCoor, tam_lista);
		
		}
		
		exit(EXIT_SUCCESS);
		
		
	}

