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
	while(separador(str[i]) == 1)
	{
			i++;
	}
	if (str[i] != '\0'){
	args[palabra] = & str[i];
	palabra++;
	}
								
	for ( ; (str[i] != '\0') && (palabra - 1 < maxargs) ; i++) 
	{
						
		if (separador(str[i]) == 1) {
			str[i] = '\0';
			i++;
			while (separador(str[i]) == 1)
			{
				i++;
			}
			if(str[i] != '\0' && palabra - 1 < maxargs ){
					
					args[palabra] = & str[i];
					palabra++;
			}
			
							
								
		}
							
	}
			
			
		
			
	//palabra++;
	return palabra;

}


int 
main (void)
 {
	
	char s[] = " hola   diego          ";
	int n;
	
	char *args[64];
	n= mytokenize(s, args, 3);
	printf("Tokens : %d\n", n);
	
	
	exit(EXIT_SUCCESS);
	
}
