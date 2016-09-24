/*Alumno Oscar Marcelo Caballero*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//#include<ctype.h>



#define TAMCOMPLEX 		20  // longitud del componente lexema 
#define TAMLEX			50		// tamaño del lexema


FILE *archivo;			// archivo Fuente 
FILE *salida;			// archivo salida//


char id[TAMLEX];		// Utilizado por el analizador lexico
int numLinea=1;

struct token {
	char componente_lexico[TAMCOMPLEX];
	char lexema[TAMLEX];
	
}t;

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}


void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[100];
	struct token e;


	while((c=fgetc(archivo))!=EOF)
	{
		
		if (c==' ' || c=='\t'){
			fprintf(salida,"%c",c);
			continue;	
		}	
		else if(c=='\n')
		{
			numLinea++;
			fprintf(salida,"%c",c);
			continue;

		} 
/*Reconocimiento del string------------------------------------------------------------------------------------*/
		else if(c == '"'){
			i=0;
			id[i]=c;
			c=fgetc(archivo);
			while(c!='"' && c!='\n'){
				if(i <= (TAMLEX-3)){
					i++;
					id[i]=c;
				}else{
					error("Longitud del string excede tamaño de buffer, se ignora la lectura");
					/*Busca el salto de linea,*/					
					while(c != '\n'){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado");
							exit(1);
						}
					}
					
					break;
				}

				c = fgetc(archivo);
				
			
			}

			if(c == '"'){
				id[i+1] = '"';
				id[i+2] = '\0';
				strcpy(t.componente_lexico,"STRING");
				strcpy(t.lexema,id); 
			}
			if(c == '\n'){
				error("Se encontro salto de linea, se ignora la lectura");
				strcpy(t.componente_lexico,"");
				stpcpy(t.lexema,"");
				ungetc(c,archivo);
			}
			c=0;
			break;
	
		}

	
/*Reconociendo numeros-------------------------------------------------------------------------------------------*/
		else if (isdigit(c))
		{
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito 
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							error(msg);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						
						strcpy(t.componente_lexico,"NUMBER");
						strcpy(t.lexema,id);
						
						break;
					/*En caso de error en la lectura del numero*/
					case -1:
						if(c != EOF){
							/*Busca el salto de linea*/
							while(c != '\n'){
								c=fgetc(archivo);
									if(c == EOF){
										error("Fin de archivo inesperado");
										exit(-1);
									}
							}
				
							ungetc(c,archivo);
							strcpy(t.componente_lexico,"");
							strcpy(t.lexema,"");
						}else{ 
								error("Fin de archivo");
								exit(1);
						}	
						acepto=1;
						c=0;
						break;
					}
					/*Controla que el numero leido no exceda el tamaño del buffer*/
					if(i==TAMLEX-2){
							sprintf(msg,"Longitud del numero excede el tamaño de buffer, se almacenara hasta %d cifras",TAMLEX-1);
							error(msg);
							while(c != '\n'){
								c=fgetc(archivo);
								if(c == EOF){
									error("Fin de archivo inesperado");
								exit(1);
								}
							
							}
							ungetc(c,archivo);
							id[++i]='\0';
							strcpy(t.componente_lexico,"NUMBER");
							strcpy(t.lexema,id);
							break;				
					}
				}
			break;
		}
/*Reconocimiento de palabras reservadas--------------------------------------------------------------------------*/
		
		else if(isalpha(c)){
			i=0;
			id[i]=c;
			c=fgetc(archivo);
			while(isalpha(c)){
				if(i <= (TAMLEX-2)){
					i++;
					id[i]=c;
				}else{
					sprintf(msg,"Longitud de la palabra reservada excede tamaño de buffer, se almacenara hasta %d caracteres",TAMLEX-1);
					error(msg);
					/*Busca el salto de linea*/
					while(c != '\n'){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado");
							exit(1);
						}
					}
					
					break;
				}

				c = fgetc(archivo);
				
			
			}
			id[i+1] = '\0';
		    
			
			if (strcmp("false",id) == 0 || strcmp("FALSE",id) == 0){
				strcpy(t.componente_lexico, "PR_FALSE");/*aqui deberia escribir el archivo*/
				strcpy(t.lexema,id);
			}else if (strcmp("true",id) == 0 || strcmp("TRUE",id) == 0){
				strcpy(t.componente_lexico,"PR_TRUE");/*aqui*/
				strcpy(t.lexema,id);
			}else if (strcmp("null",id) == 0||strcmp("NULL",id) == 0){
				strcpy(t.componente_lexico,"PR_NULL");/*aqui*/
				strcpy(t.lexema,id);
			}else{
				printf("Lin %d: No se reconoce  '%s' como palabra reservada.\n" ,numLinea,id);
				/*Se busca salto de de linea*/
				while(c != '\n'){
						c=fgetc(archivo);
						if(c == EOF){
							error("Fin de archivo inesperado");
							exit(1);
						}
				}
				
				strcpy(t.componente_lexico,"");
				strcpy(t.lexema,"");	
			}
			ungetc(c,archivo);
			c=0;
			break;
		}
/*Reconocimiento de caracteres validos---------------------------------------------------------------------------*/
		else if (c==':')
		{
			strcpy(t.componente_lexico,"DOS_PUNTOS");
			stpcpy(t.lexema,":");
			break;
		}
		else if (c==',')
		{
			strcpy(t.componente_lexico,"COMA");
			stpcpy(t.lexema,",");
			break;
		}
		else if (c=='{')
		{
			strcpy(t.componente_lexico,"L_LLAVE");
			stpcpy(t.lexema,"{");
			break;
		}
		else if (c=='}')
		{
			strcpy(t.componente_lexico,"R_LLAVE");
			stpcpy(t.lexema,"}");
			break;
		}
		else if (c=='[')
		{
			strcpy(t.componente_lexico,"L_CORCHETE");
			stpcpy(t.lexema,"[");
			break;
		}
		else if (c==']')
		{
			strcpy(t.componente_lexico,"R_CORCHETE");
			stpcpy(t.lexema,"[");
			break;
		}
/*Descartando  caracteres no validos-----------------------------------------------------------------------------*/
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
			/*Busca salto de linea*/
			while(c != '\n'){
					c=fgetc(archivo);
					if(c == EOF){
						error("Fin de archivo inesperado");
						exit(1);						}
				}
			ungetc(c,archivo);
			strcpy(t.componente_lexico,"");
			strcpy(t.lexema,"");	
			
			
		}
	}
/*Finalizacion del archivo---------------------------------------------------------------------------------------*/
	if (c==EOF)
	{
		strcpy(t.componente_lexico,"EOF");
		strcpy(t.lexema,"EOF");

	}
	
}



/**/
int main (int argc,char* args[]){
	
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo fuente no encontrado.\n");
			exit(1);
		}
		salida = fopen("salida.txt","w");
		sigLex();
		while (strcmp(t.componente_lexico,"EOF") != 0){
			fprintf(salida," %s ",t.componente_lexico);
			sigLex();

		}
		fclose(archivo);
		fclose(salida);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
	
}
