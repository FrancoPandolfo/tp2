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

void ordenar_archivo(char* archin, char *archout, size_t capacidad){
	//abro ambos archivos
	FILE * entrada = fopen (archin,"r");
	if (!entrada){
		fprintf(stderr, "%s", "Archivo erroneo\n");
		return;
	} 
	FILE * salida = fopen (archout,"w");
	if (!salida){
		fclose(entrada);
		fprintf(stderr, "%s", "Error en Creacion de Archivo\n");
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
		
			//escribo a archivo
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
	lista_t* lista = lista_crear();

	for (int i = 0; i <num_arch - 1; i++){
		char filename[7];
    	sprintf(filename, "%d.txt", (i + 1));
   	 	FILE*faux = fopen(filename, "r");

		size_t cant2 = 0;
		char*linea2 = NULL;
		ssize_t leidos2 = 0;
   	 	while (leidos2 != -1){ 
   	 		leidos2 = getline (&linea2,&cant2,faux);
    		if (leidos2 != -1){ 
    		 	lista_insertar_ultimo(lista,linea2);
    		 	linea2 = NULL;
    		 }
		}
		free(linea2);
		while(!lista_esta_vacia(lista)){ 
			char*linea_aux = lista_borrar_primero(lista);
			fprintf(salida,"%s", linea_aux);
			free(linea_aux);
		}
		fclose(faux);
	}
	fclose(salida);
	//borro archivos auxiliares
	for (int i = 1; i < num_arch; i++){
		char filename[7];
    	sprintf(filename, "%d.txt", i);
		remove(filename);
	}
	lista_destruir(lista,free);
}


void agregar_archivo(char*archivo){
	FILE * entrada = fopen (archivo,"r");
	if (!entrada){
		fprintf(stderr, "%s", "Archivo erroneo\n");
		return;
	} 

	hash_t* hash = hash_crear(free);

	size_t cant = 0;
	char*linea = NULL;
	ssize_t leidos = 0;
	//null terminated
	while(leidos != -1){
		leidos = getline (&linea,&cant,entrada);
		if (leidos != -1){ 
			// hago un split a linea
			char**registro = split(linea,'\t');
			//guardo ip como clave y linea como dato
			//en el hash se van a guardar todos los mismos ips en una misma linea. la clave es el ip y el dato el registro
			char*ip_guardar = registro[0];
			hash_guardar(hash,ip_guardar,linea);
			free_strv(registro);
			linea = NULL;
		}
	} 
	fclose(entrada);
	free(linea);
	int dos = 1;

	hash_iter_t* iter1 = hash_iter_crear(hash);
	const char*ip1 = hash_iter_ver_actual(iter1);
	char*linea1 = hash_obtener(hash,ip1);
	char**registro1 = split(linea1,'\t');
	time_t tiempo1 = iso8601_to_time(registro1[1]);
	free_strv(registro1);

	hash_iter_t* iter2 = hash_iter_crear(hash);
	hash_iter_avanzar(iter2);
	const char*ip2 = hash_iter_ver_actual(iter2);
	char*linea2 = hash_iter_ver_actual_dato(iter2);
	char**registro2 = split(linea2,'\t');
	time_t tiempo2 = iso8601_to_time(registro2[1]);
	free_strv(registro2);

	int contador = 0;
	while (hash_cantidad(hash) > contador){ 
		while(strcmp(ip1,ip2) == 0){
			if (fabs(difftime(tiempo1,tiempo2)) <= 2){
				dos++;
				if (dos == 5){
					printf("DoS: %s\n",ip1);
					dos = 1;
					break;
				}
				hash_iter_avanzar(iter2);
				ip2 = hash_iter_ver_actual(iter2);
				linea2 = hash_iter_ver_actual_dato(iter2);
				registro2 = split(linea2,'\t');
				tiempo2 = iso8601_to_time(registro2[1]);
				free_strv(registro2);
				contador++;
			}
			else{
				hash_iter_avanzar(iter1);
				ip1 = hash_iter_ver_actual(iter1);
				linea1 = hash_iter_ver_actual_dato(iter1);
				registro1 = split(linea1,'\t');
				tiempo1 = iso8601_to_time(registro1[1]);
				free_strv(registro1);

				hash_iter_avanzar(iter2);
				ip2 = hash_iter_ver_actual(iter2);
				linea2 = hash_iter_ver_actual_dato(iter2);
				registro2 = split(linea2,'\t');
				tiempo2 = iso8601_to_time(registro2[1]);
				free_strv(registro2);
			}
		}
		if (strcmp(ip1,ip2) != 0){ 
			for(int i = 0; i < contador; i++){
				hash_iter_avanzar(iter1);
				ip1 = hash_iter_ver_actual(iter1);
				linea1 = hash_iter_ver_actual_dato(iter1);
				registro1 = split(linea1,'\t');
				tiempo1 = iso8601_to_time(registro1[1]);
				free_strv(registro1); 
			}
		}
		contador = 0;
		//caso de break
		while(strcmp(ip1,ip2) == 0){
			hash_iter_avanzar(iter1);
			ip1 = hash_iter_ver_actual(iter1);

			hash_iter_avanzar(iter2);
			ip2 = hash_iter_ver_actual(iter2);
		}
		dos = 1;
		//cuando sale del while tengo que avanzar una posicion mas con ambos iter
		hash_iter_avanzar(iter1);
		ip1 = hash_iter_ver_actual(iter1);
		linea1 = hash_iter_ver_actual_dato(iter1);
		registro1 = split(linea1,'\t');
		tiempo1 = iso8601_to_time(registro1[1]);
		free_strv(registro1);

		hash_iter_avanzar(iter2);
		if (!hash_iter_al_final(iter2)){ 
			ip2 = hash_iter_ver_actual(iter2);
			linea2 = hash_iter_ver_actual_dato(iter2);
			registro2 = split(linea2,'\t');
			tiempo2 = iso8601_to_time(registro2[1]);
			free_strv(registro2); 
		}
		else break;
	}
	hash_iter_destruir(iter1);
	hash_iter_destruir(iter2);
	hash_destruir(hash);
}

void ver_vistitantes(char* desde, char* hasta, abb_t* abb){

	abb_iter_t * iter = abb_iter_in_crear(abb);
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
}


int main(int argc, char*argv[]){

	if (argc == 2){
		size_t capacidad_maxima = atoi(argv[1]);
		abb_t* abb = abb_crear(comparacion3,free);
		size_t cant = 0;
		char*linea = NULL;
		char* parametro1;
		char* parametro2;
		char* parametro3;
		while(true){ 
			getline (&linea,&cant,stdin);
			char**lineas = split(linea,' ');
			int cant_ing = strv_cant(lineas);
			parametro1 = lineas[0];
			if (cant_ing == 2){
				parametro2 = lineas[1];
			}
			if (cant_ing == 3){
				parametro2 = lineas[1];
				parametro3 = lineas[2];
			}
			if ( (strcmp(parametro1,"agregar_archivo") == 0)) {
				agregar_archivo(parametro2);
				printf("OK\n");
				//abrir argv[2] sacar por split los ips y guardarlos en un abb
				FILE* archivo = fopen(parametro2,"r");
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
						linea2 = NULL;
					}
				}
				free(linea2);
				fclose(archivo);
			}

			if ( (strcmp(parametro1,"ver_visitantes") == 0) || (strcmp(parametro1,"ordenar_archivo") == 0) ){ 
				if (strcmp(parametro1,"ver_visitantes") == 0){
					ver_vistitantes(parametro2,parametro3,abb);
					printf("OK\n");
				}
				if (strcmp(parametro1,"ordenar_archivo") == 0){
					ordenar_archivo(parametro2,parametro3,capacidad_maxima);
					printf("OK\n");
				}
			}
			if ((strcmp(parametro1,"agregar_archivo") != 0) && (strcmp(parametro1,"ver_visitantes") != 0) && (strcmp(parametro1,"ordenar_archivo") != 0) ){
				fprintf(stderr, "%s %s\n", "Error en comando", parametro1);
				free(linea);
				free_strv(lineas);
				abb_destruir(abb);
				return 0;
			}
			free(linea);
			linea = NULL;
			free_strv(lineas);
		}
	}
	else{
		fprintf(stderr, "%s %s\n", "Error en comando", argv[0]);
	}
}