/* EJERCICIO: 1 
 * 
 * Jhon Fajardo Rodas  Tecnologías*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int 
separador (char  c) 
{
		if (c == '\r'){
			return 1;
		}else if (c == '\t'){
			return 1;
		}else if (c == '\n'){
			return 1;
		}else if (c == ' '){
			return 1;
		}else {
			return 0;
		}
}

int 
mytokenize (char *str, char **args, int maxargs)
{	
	int i;
	int palabra;
	palabra = 0;
	i=0;
	while(separador(str[i]))
	{
			i++;
	}
	if (str[i] != '\0'){
		args[palabra] = & str[i];
		palabra++;
	}
	

	for ( ; (str[i] != '\0') && (palabra < maxargs) ; i++) 
	{				
		if (separador(str[i])) {
			str[i] = '\0';
			i++;
			while (separador(str[i]))
			{
				i++;
			}
			if(str[i] != '\0' && palabra  < maxargs){
					
				args[palabra] = & str[i];
				palabra++;
			}


		}		
	}
	
	return palabra;
}


int 
main (void)
 {
	char *args[64];
	int n;
	char s[] = "que pasa cara pasa";
	int maxargs = 6;
		
	n= mytokenize(s, args, maxargs);
	printf("Numero de tokens  : [%d tokens]\n", n);
	int i = 0;
	while(i<n)
	{
		printf("token-- %d => %s\n", i, args[i]);
		i++;
	}


	int maxargs2 = 3;
	char *args2[64];
	char s2[] = "hola mundo, lo pongo en mi tercera asignatura de programacion";
	int k= mytokenize(s2, args2, maxargs2);
	printf("Numero de tokens  : [%d tokens]\n", k);
	int j = 0;
	while(j<k)
	{
		printf("token-- %d => %s\n", j, args2[j]);
		j++;
	}
	exit(EXIT_SUCCESS);
	
}
