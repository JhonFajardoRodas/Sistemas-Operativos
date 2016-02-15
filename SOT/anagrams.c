/* EJERCICIO: 2
 * 
 * Jhon Fajardo Rodas  Tecnologías*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 
 int 
 
 completo (char *str)
 {
	 
	int i;
	int ok = 0;
	int completo;
	
	 for (i = 0; i < strlen(str); i++)
	 {
		 if (str[i] == '-'){
			 ok++;
		 }
	 }
	 
	 if (ok == strlen(str)){
		completo = 1; 
	 }else{
		completo = 0;
	 }
	 
	 return completo;
 }

 
	
int 
anagrama (char *str1, char *str2)
{
	
	int anagrama; 
	
	int i;
	int j;
	char aux1[128];
	char aux2[128];
	
	strcpy(aux1, str1);
	strcpy(aux2, str2);
	
	
	
	if (strlen(aux1) == strlen(aux2)){
		for (i = 0; i < strlen(aux1); i++){
			for (j = 0; j < strlen(aux2)  ; j ++){
				if (aux1[i] == aux2[j] && (aux2[j] != '-')){
					aux2[j] = '-';
					aux1[i] = '-';
					//printf("%s\n", aux2); 
				
				}
			}
		}
		
		if (completo(aux2)){
			//printf("Esta lleno \n") ;
			anagrama = 1;
			
		}else {
			//printf("No esta lleno por lo tanto NO es anagrama \n") ;
			anagrama = 0;
		}
	}else{
		anagrama = 0;
		//printf("NO es anagrama \n") ;
	}
	
	//printf("a = %d\n", anagrama);
	return anagrama;
}
 
  int 
 main (int argc, char *argv[])
 {
	int a;
	int i;
	int j;
	int k;
	int *estados;
	char l;
	int pintar;
	int m;
	int encontradoanagrama=0;

	estados=malloc(sizeof(int)*argc);
	memset(estados,0,sizeof(int)*argc); //inicialiazo toda la memoria a 0


	 for (i= 1; i < argc; i++)
	 {
		if (estados[i]!=0)
			continue;
		encontradoanagrama=0;
		 for (j = i+1; j < argc  ; j++)
		 {
			 a = anagrama(argv[i], argv[j]);
			 if (estados[j]==0){
				 if (a == 1) {
				 	encontradoanagrama=1;
					estados[i]=1;
					estados[j]=1;
				
				 }
			}
			
		 }
		 if(encontradoanagrama){
		 	//ahora vamos a sacar los anagramas
			 for(k=0;k<argc;k++){
			 	if (estados[k]==1){
			 		printf("%s ",argv[k]);
			 		
			 	}
			 }
			 printf("[");
			 //ahora vamos a sacar las letras repetidas
			 for(k=0;k<strlen(argv[i]);k++)
			 {
			 		l=argv[i][k];
			 		pintar=0;
			 		for(m=0;m<argc;m++){
			 			if (estados[m]==1){
			 				if(l==argv[m][k])
			 					pintar=1;
			 				else{
			 					pintar=0;
			 					break;
			 				}
			 			}

			 		}
			 		if(pintar)
			 			printf("%c",l);

			 }
			 printf("]");
			 printf("\n");
			 for(k=0;k<argc;k++)
			 {
			 	if (estados[k]==1)
			 		estados[k]=-1;

			 }
		}
	 }
	
	 //free (list);
	 exit(EXIT_SUCCESS);
	 
 }
	 