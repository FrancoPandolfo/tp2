#define _POSIX_C_SOURCE 20089L
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#define __USE_XOPEN
#include "tp2.h"
#include "lista.h"
#include "hash.h"
#include "heap.h"
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>


#define TIME_FORMAT "%FT%T%z"
// 1 kilobyte = 1024 bytes = 8192 bits
//#define CAP_MAX  409600
//409600 bytes = 400 kilobytes
#define TAM_MAX_LINEA  250
#define BYTE_SIZE  1024
//#define CANT_REGISTROS ((CAP_MAX / 4) / TAM_MAX_LINEA)
#define TAM_MIN 25
#define TAM_IP  16
#define TAM_PRED 500
#define MUCHO 100000

typedef struct ordenamiento{
	int arch;
	char* line;
}ordenamiento_t;

ordenamiento_t *ordenamiento_crear(char *line, int arch){
	ordenamiento_t* orden = malloc(sizeof(ordenamiento_t));
	if(!orden) return orden;
	orden->arch = arch;
	orden->line = line;
	return orden;
}

void orden_destruir(ordenamiento_t*orden){
	free(orden->line);
	free(orden);
}

void destruir_orden(void*dato){
	orden_destruir((ordenamiento_t*)dato);
}


time_t iso8601_to_time(const char* iso8601){
    struct tm bktime = { 0 };
    strptime(iso8601, TIME_FORMAT, &bktime);
    return mktime(&bktime);
}

/*************************************************	
 *			Bloque para la función cmp			 *
 *************************************************/
int diff_time(char *str_time_a, char *str_time_b){
	time_t time_a = iso8601_to_time(str_time_a);
	time_t time_b = iso8601_to_time(str_time_b);

	if(time_a == time_b) return 0;
	if(time_a > time_b) return -1;
	else
		return 1;
}

int ip_cmp(char **strv_ip_a, char **strv_ip_b){
	size_t i = 0;
	while(strv_ip_a[i] && strv_ip_b[i]){
		if(atoi(strv_ip_a[i]) > atoi(strv_ip_b[i])) return -1;
		if(atoi(strv_ip_a[i]) < atoi(strv_ip_b[i])) return 1;
		i++;
	}
	return 0;
}

int diff_ip(char *str_ip_a, char *str_ip_b){
	char **strv_ip_a = split(str_ip_a, '.');
	char **strv_ip_b = split(str_ip_b, '.');
	int res_ip = ip_cmp(strv_ip_a, strv_ip_b);

	free_strv(strv_ip_a);
	free_strv(strv_ip_b);

	return res_ip;
}

int cmp(const void*a, const void *b){
	char **strv_a = split((char *)a, '\t');
	char **strv_b = split((char *)b, '\t');

	int num_time = diff_time(strv_a[1], strv_b[1]);
	int num_ip = diff_ip(strv_a[0], strv_b[0]);
	int num_rec = -1*strcmp(strv_a[3], strv_b[3]);

	free_strv(strv_a);
	free_strv(strv_b);

	if(num_time) return num_time;
	if(num_ip) return num_ip;
	return num_rec;
}

/*************************************************	
 *		Fin del bloque de la función cmp		 *
 *************************************************/

int comparacion2(char*linea1, char*linea2){
	char **strv_ip_a = split(linea1, '.');
	char **strv_ip_b = split(linea2, '.');
	int res_ip = ip_cmp(strv_ip_a, strv_ip_b);

	free_strv(strv_ip_a);
	free_strv(strv_ip_b);

	return res_ip;
}

int cmp2(const void *n1, const void *n2){
	return comparacion2((char*)n1, (char *)n2);
}

int comparacion3(const char*linea1,const char*linea2){
	char **strv_ip_a = split(linea1, '.');
	char **strv_ip_b = split(linea2, '.');
	int res_ip = ip_cmp(strv_ip_a, strv_ip_b);

	free_strv(strv_ip_a);
	free_strv(strv_ip_b);

	return res_ip;
}

int cmp3(const void*a, const void *b){
	ordenamiento_t* orden1 = (ordenamiento_t*)a;
	ordenamiento_t* orden2 = (ordenamiento_t*)b;

	char **strv_a = split(orden1->line, '\t');
	char **strv_b = split(orden2->line, '\t');

	int num_time = diff_time(strv_a[1], strv_b[1]);
	int num_ip = diff_ip(strv_a[0], strv_b[0]);
	int num_rec = -1*strcmp(strv_a[3], strv_b[3]);

	free_strv(strv_a);
	free_strv(strv_b);

	if(num_time) return num_time;
	if(num_ip) return num_ip;
	return num_rec;
}

void ordenar_archivo(char* archin, char *archout, size_t capacidad){
	//abro ambos archivos
	FILE * entrada = fopen (archin,"r");
	if (!entrada){
		fprintf(stderr, "%s %s\n", "Error en comando", "ordenar_archivo");
		return;
	} 
	FILE * salida = fopen (archout,"w");
	if (!salida){
		fclose(entrada);
		fprintf(stderr, "%s %s\n", "Error en comando", "ordenar_archivo");
		return;
	}

	heap_t* heap = heap_crear(cmp);
	//calculo cantidad de registros maxima por iteracion
	size_t CANT_REGISTROS = ((capacidad) * BYTE_SIZE) / TAM_MAX_LINEA;
	size_t cant = 0;
	char*linea = NULL;
	ssize_t leidos = 0;
	int cont = 0;     //contador de iteraciones
	int num_arch = 1; //numero de archivos auxiliares
	int cant_reg_aux = 0; //cantidad de registros por iteracion
	int cant_reg = 0; //cantidad registros totales
	while (leidos != -1){
		//leo y guardo en heap de minimos
		while( (cont < CANT_REGISTROS) && (leidos != -1)){
			//null terminated
			leidos = getline (&linea,&cant,entrada);
			if (leidos != -1){
				heap_encolar(heap,linea);
				cant_reg++;
				cant_reg_aux++;
				linea = NULL;
			} 
			cont++;
		}
		free(linea);
		if (!heap_esta_vacio(heap)){ 
			//abro archivo auxiliar
    		char filename[7];
    		sprintf(filename, "%d.txt", num_arch);
    		FILE* file = fopen(filename, "w");
    		if (!file){
    			fclose(entrada);
    			fclose(salida);
    			return;
    		}
    		num_arch++;
		
			//escribo a archivo auxiliar
			for(int i = 0; i < cant_reg_aux; i++){
				char*linea_aux = heap_desencolar(heap);
				fprintf(file,"%s",linea_aux );
				free(linea_aux);
			}
			fclose(file);
		}
		cant_reg_aux = 0;
		cont = 0;
	}
	fclose(entrada);
	heap_destruir(heap,free);
	heap_t* heap2 = heap_crear(cmp3);
	FILE* files[num_arch - 1];
	int f = 0;
	for (int i = 0; i < num_arch - 1; i++){
		char filename[7];
    	sprintf(filename, "%d.txt", (i + 1));
   	 	files[i] = fopen(filename, "r");
   	 	f++;
   	 	if (!files[i]){
   	 		if (f != 0){ 
   	 			for (int j = f - 1; j > -1; j--){
   	 				fclose(files[j]);
   	 			}
   	 		}
			fclose(salida);
			fprintf(stderr, "%s %s", "Error en comando", "ordenar_archivo\n");
			return;
		}
	}

	ssize_t leidos2 = 0;
	size_t cant2 = 0;
	char*linea2 = NULL;
	int num_arch2 = num_arch;
	//leo las primeras k lineas
	for(int k = 0; k < num_arch2 - 1; k++){ 
		leidos2 = getline (&linea2,&cant2,files[k]);
    	if (leidos2 != -1){
    		ordenamiento_t * orden = ordenamiento_crear(linea2,k);
    		heap_encolar(heap2,orden);
    		linea2 = NULL;
    	}
    	else{
    		num_arch2--;
    		free(linea2);
    		linea2 = NULL;
    	}
	}

	int k;
	for (int i = 0; i < cant_reg; i++){ 
		if(!heap_esta_vacio(heap2)){ 
			ordenamiento_t* orden_aux;
			orden_aux = heap_desencolar(heap2);
			char*linea_aux = orden_aux->line;
			fprintf(salida,"%s", linea_aux);
			k = orden_aux->arch;
			orden_destruir(orden_aux);
			leidos2 = getline (&linea2,&cant2,files[k]);
    		if (leidos2 != -1){
    			ordenamiento_t * orden = ordenamiento_crear(linea2,k);
    			heap_encolar(heap2,orden);
    			linea2 = NULL;
    		}
    		else{
    			free(linea2);
    			linea2 = NULL;
    		}
		}
	}
	free(linea2);
	fclose(salida);
	for (int i = 0; i < num_arch - 1; i++){
		fclose(files[i]);
	}
	//borro archivos auxiliares
	for (int i = 1; i < num_arch; i++){
		char filename[7];
    	sprintf(filename, "%d.txt", i);
		remove(filename);
	}
	heap_destruir(heap2,destruir_orden);
	printf("OK\n");
}


char *linea_obtener(FILE *archivo){
	char *linea = NULL;
	size_t capacidad = 0;
	if(getline(&linea, &capacidad, archivo) == -1){
		free(linea);
		return NULL;
	}
	return linea;
}

void free_lista(void *dato){
	lista_t *lista = (lista_t *)dato;
	lista_destruir(lista, free);
}

/*	Determina si en lista pasada con clave ocurre DoS	*/
bool DoS(lista_t *lista){
	lista_iter_t *iter = lista_iter_crear(lista);
	lista_iter_t *iter_movil = lista_iter_crear(lista);

	/*	Posiciono el iter móvil	*/
	for(size_t i = 0; i < 4; i++){
		lista_iter_avanzar(iter_movil);
	}

	/*	Evalúo si hay DoS	*/
	bool ok = false;
	while(!lista_iter_al_final(iter_movil)){
		time_t tiempo_a = iso8601_to_time((char *)lista_iter_ver_actual(iter));
		time_t tiempo_b = iso8601_to_time((char *)lista_iter_ver_actual(iter_movil));
		ok = fabs(difftime(tiempo_a, tiempo_b)) < 2;		
		if(ok) break;
		lista_iter_avanzar(iter);
		lista_iter_avanzar(iter_movil);
	}
	lista_iter_destruir(iter);
	lista_iter_destruir(iter_movil);
	return ok;
}

bool ip_mostrar(const char *clave, void *dato, void *extra){
	//if(!clave) return false;
	fprintf(stdout, "DoS: %s\n", clave);
	return true;
}

void reporte_DoS(abb_t *arbol){
	abb_in_order(arbol, ip_mostrar, NULL);
	printf("OK\n");
}

/*	Función auxiliar para comparar dentro del abb	*/
int ip_cmp_aux(const char *ip_a, const char *ip_b){
	return diff_ip((char *)ip_b, (char *)ip_a);
}

void agregar_archivo(char *ruta){
	FILE *archivo = fopen(ruta, "r");
	if (!archivo){
		fprintf(stderr, "%s %s\n", "Error en comando", "agregar_archivo");
		return;
	} 

	/*	Declaro los TDAs a utilizar	*/
	hash_t *hash = hash_crear(free_lista);
	abb_t *arbol = abb_crear(ip_cmp_aux, free);

	/*	Usamos las IPs como claves del hash	*/
	while(!feof(archivo)){
		char *linea = linea_obtener(archivo);
		if(linea){
			char **linea_strv = split(linea, '\t');
			char *ip = linea_strv[0];
			char *tiempo = linea_strv[1];

			if(!hash_pertenece(hash, ip)){
				lista_t *lista = lista_crear();
				hash_guardar(hash, ip, lista);
			}
			lista_t *lista = hash_obtener(hash, ip);
			lista_insertar_ultimo(lista, strdup(tiempo));
			free_strv(linea_strv);
		}
		free(linea);
	}

	/*	Chequeo por cada clave, si hay DoS	*/
	hash_iter_t *iter = hash_iter_crear(hash);

	while(!hash_iter_al_final(iter)){
		char *ip = (char *)hash_iter_ver_actual(iter);
		lista_t *lista = (lista_t *)hash_obtener(hash, ip);
		if(DoS(lista)){
			abb_guardar(arbol, ip, NULL);
		}
		hash_iter_avanzar(iter);
	}

	reporte_DoS(arbol);

	/*	Cierro archivo y borro datos de estructuras auxiliares	*/
	fclose(archivo);
	hash_iter_destruir(iter);
	hash_destruir(hash);
	abb_destruir(arbol);
}

void ver_vistitantes(char* desde, char* hasta, abb_t* abb){

	abb_iter_t * iter = abb_iter_in_crear(abb);
	//uso heap para imprimir los ip en orden
	heap_t* heap = heap_crear(cmp2);

	char**dess = split(desde,'.');
	char**hass = split(hasta,'.');

	const char*ip = abb_iter_in_ver_actual(iter);
	char**ips = split(ip,'.');

	printf("Visitantes:\n");
	while (!abb_iter_in_al_final(iter)){ 
		if ((ip_cmp(ips,dess) <= 0) && (ip_cmp(ips,hass) >= 0)){ 
			heap_encolar(heap,(char*)ip);
			abb_iter_in_avanzar(iter);
			if (!abb_iter_in_al_final(iter)){ 
				free_strv(ips);
				ip = abb_iter_in_ver_actual(iter);
				ips = split(ip,'.');
			}
		}
		else{ 
			abb_iter_in_avanzar(iter);
			if (!abb_iter_in_al_final(iter)){
				free_strv(ips);
				ip = abb_iter_in_ver_actual(iter);
				ips = split(ip,'.');
			}
		}
	}
	while (!heap_esta_vacio(heap)){
		char*imprimir = heap_desencolar(heap);
		printf("\t%s\n",imprimir);
	}
	free_strv(ips);
	free_strv(dess);
	free_strv(hass);
	abb_iter_in_destruir(iter);
	heap_destruir(heap,free);
	printf("OK\n");
}


int main(int argc, char*argv[]){

	if (argc == 2){
		size_t capacidad_maxima = atoi(argv[1]);
		//el abb va a guardar los ip de todos los archivos agregados
		abb_t* abb = abb_crear(comparacion3,NULL);
		size_t cant = 0;
		char*linea = NULL;
		ssize_t leidos_stdin;
		char* parametro1;
		char* parametro2;
		char* parametro3;
		while((leidos_stdin = getline (&linea,&cant,stdin)) > 0){ 
			char**lineas = split(linea,' ');
			int cant_ing = strv_cant(lineas);
			parametro1 = lineas[0];
			if (cant_ing == 2){
				parametro2 = lineas[1];
				size_t len = strlen(lineas[1]);
				parametro2[len - 1] = '\0';
			}
			if (cant_ing == 3){
				parametro2 = lineas[1];
				parametro3 = lineas[2];
				size_t len = strlen(lineas[2]);
				parametro3[len - 1] = '\0';
			}
			if (cant_ing > 3){
				fprintf(stderr, "%s %s%c", "Error en comando", parametro1,'\n');
				free(linea);
				free_strv(lineas);
				abb_destruir(abb);
				return 0;
			}
			if ( (strcmp(parametro1,"agregar_archivo") == 0)) {
				if (cant_ing > 2){
					fprintf(stderr, "%s %s%c", "Error en comando", parametro1,'\n');
					free(linea);
					free_strv(lineas);
					abb_destruir(abb);
					return 0;
				}
				agregar_archivo(parametro2);
				//abrir argv[2] sacar por split los ips y guardarlos en un abb
				FILE* archivo = fopen(parametro2,"r");
				if (!archivo){
					free(linea);
					free_strv(lineas);
					abb_destruir(abb);
					return 0;
				} 
				size_t cant2 = 0;
				char*linea2 = NULL;
				ssize_t leidos = 0;
				while (leidos != -1){
					leidos = getline (&linea2,&cant2,archivo);
					if (leidos != -1){ 
						char**string = split(linea2,'\t');
						char*ip_guardar = string[0];
						abb_guardar(abb,ip_guardar,NULL);
						free_strv(string);
						free(linea2);
						linea2 = NULL;
					}
				}
				free(linea2);
				fclose(archivo);
				free_strv(lineas);
				free(linea);
				linea = NULL;
			}

			else if ( (strcmp(parametro1,"ver_visitantes") == 0) || (strcmp(parametro1,"ordenar_archivo") == 0) ){ 
				if (cant_ing < 3){
					fprintf(stderr, "%s %s%c", "Error en comando", parametro1,'\n');
					free(linea);
					free_strv(lineas);
					abb_destruir(abb);
					return 0;
				}
				if (strcmp(parametro1,"ver_visitantes") == 0){
					if (abb_cantidad(abb) == 0){
						fprintf(stderr, "%s %s%c", "Error en comando", parametro1,'\n');
						free(linea);
						free_strv(lineas);
						abb_destruir(abb);
						return 0;
					}
					ver_vistitantes(parametro2,parametro3,abb);
				}
				if (strcmp(parametro1,"ordenar_archivo") == 0){
					ordenar_archivo(parametro2,parametro3,capacidad_maxima);
					FILE* archivo = fopen(parametro2,"r");
					if (!archivo){
						free(linea);
						free_strv(lineas);
						abb_destruir(abb);
						return 0;
					}
					else fclose(archivo);
				}
				free_strv(lineas);
				free(linea);
				linea = NULL;
			}
			else {
				fprintf(stderr, "%s %s", "Error en comando", parametro1);
				free(linea);
				free_strv(lineas);
				abb_destruir(abb);
				return 0;
			}
		}
		free(linea);
		abb_destruir(abb);
	}
	else{
		fprintf(stderr, "%s %s%c", "Error en comando", argv[0],'\n');
	}
}