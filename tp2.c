#define _POSIX_C_SOURCE 20089L
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#define __USE_XOPEN
#include "tp2.h"
#include "lista.h"
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
	char**lineas1 = split(linea1,'\t');
	char**lineas2 = split(linea1,'\t');
	char*ip1 = lineas1[0];
	char*ip2 = lineas2[0];
	char**ips1 = split(ip1,'.');
	char**ips2 = split(ip2,'.');
	//join fue modificado para unir sin sep
	char*ipsj1 = join(ips1);
	char*ipsj2 = join(ips2);
	size_t ip1_num = atoi(ipsj1);
	size_t ip2_num = atoi(ipsj2);

	if (ip1_num == ip2_num){
		free(lineas1), free(lineas2), free(ips1), free(ips2), free(ipsj1), free(ipsj2);
		return 0;
	}
	else if (ip1_num < ip2_num){
		free(lineas1), free(lineas2), free(ips1), free(ips2), free(ipsj1), free(ipsj2);
		return 1;
	}
	else{
		free(lineas1), free(lineas2), free(ips1), free(ips2), free(ipsj1), free(ipsj2);
		return -1;
	}
}

int cmp2(const void *n1, const void *n2){
	return comparacion2((char*)n1, (char *)n2);
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
	size_t CANT_REGISTROS = (capacidad * BYTE_SIZE) / TAM_MAX_LINEA;
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
			// HAY QUE LIBERAR TODA LA MEMORIA
			if (leidos != -1){
				heap_encolar(heap,linea);
				cant_reg++;
				cant_reg_aux++;
			} 
			linea = NULL;
			cont++;
		}
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
				//HAY QUE HACER MALLOC? 
				char*linea_aux = heap_desencolar(heap);
				fprintf(file,"%s",linea_aux );
			}
			fclose(file);
		}
		cant_reg_aux = 0;
		cont = 0;
	}
	fclose(entrada);
	heap_destruir(heap,free);
	FILE *files[num_arch]; //vector de archivos auxiliares
	for (int i = 0; i < num_arch - 1; i++){
    	char filename[7];
    	sprintf(filename, "%d.txt", (i + 1));
   	 	files[i] = fopen(filename, "r");
	}

	lista_t* lista = lista_crear();
	size_t cant2 = 0;
	char*linea2 = NULL;
	ssize_t leidos2 = 0;
	int contador = 0;
	while (contador < cant_reg){ 
		//encolo num_arch lineas
		for (int i = 0; i < (num_arch - 1); i++){
    		leidos2 = getline (&linea2,&cant2,files[i]);
    		if (leidos2 != -1){
    			lista_insertar_ultimo(lista,linea2);
    			contador++;
    		}
    		linea2 = NULL;
		}

		//escribo en archivo salida
		while(!lista_esta_vacia(lista)){ 
			char*linea_aux = lista_borrar_primero(lista);
			fprintf(salida,"%s", linea_aux);
		}
	}
	//cierro archivos
	for (int i = 0; i < num_arch - 1; i++){
		fclose(files[i]);
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

	lista_t* lista_tiempo = lista_crear();
	lista_t* lista_ip = lista_crear();

	size_t cant2 = 0;
	char*linea2 = NULL;
	int con = 0;
	ssize_t leidos2 = 0;
	//null terminated
	// HAY QUE LIBERAR TODA LA MEMORIA
	while((con < TAM_PRED) && (leidos2 != -1)){
		leidos2 = getline (&linea2,&cant2,entrada);
		if (leidos2 != -1){ 
			// SPLIT HACE MALLOC
			// hago un split a linea
			char**registro = split(linea2,'\t');
			//inserto el ip en lista ip
			lista_insertar_ultimo(lista_ip, registro[0]);
			//HACE FALTA EL MALLOC A aux_tiempo ?
			time_t* aux_tiemp = malloc(sizeof(time_t));
			*aux_tiemp = iso8601_to_time(registro[1]);
			lista_insertar_ultimo(lista_tiempo,aux_tiemp);
		}
		con++;
		linea2 = NULL;
	} 
	size_t cant = 0;
	char*linea = NULL;
	ssize_t leidos = 0;

	int dos = 0;

	//creo iterador de lista tiempo
	lista_iter_t* iter_tiempo = lista_iter_crear(lista_tiempo);	
	//guardo el primero de la lista en tiempo
	time_t* tiempo = lista_iter_ver_actual(iter_tiempo);
	//avanzo la lista tiempo
	lista_iter_avanzar(iter_tiempo);
	time_t* tiempo_aux = lista_iter_ver_actual(iter_tiempo);

	//crep un iterador de lista ip
	lista_iter_t* iter_ipfijo = lista_iter_crear(lista_ip);
	//guardo el primero de la lista en ip
	char*ip = lista_iter_ver_actual(iter_ipfijo);

	lista_iter_t* iter_ip = lista_iter_crear(lista_ip);
	//avanzo la lista ip
	lista_iter_avanzar(iter_ip);

	char* ult_impreso;
	heap_t* heap = heap_crear(cmp2);

	while ((leidos != -1) || (lista_largo(lista_ip) > 5)){ //FUNCIONA || lista_largo(lista_ip) > 5 ?

		if (difftime(*tiempo,*tiempo_aux) <= 2){
			if(strcmp(ip, lista_iter_ver_actual(iter_ip)) == 0){
				dos++;
				lista_iter_avanzar(iter_ip);
				lista_iter_avanzar(iter_tiempo);
				tiempo_aux = lista_iter_ver_actual(iter_tiempo);
			}
			else{
				lista_iter_avanzar(iter_ip);
				lista_iter_avanzar(iter_tiempo);
				tiempo_aux = lista_iter_ver_actual(iter_tiempo);
			}
		}

		if (dos == 5){
			//FUNCIONA LA PRIMERA COMPARACION?
			if(strcmp(ip,ult_impreso) != 0){ 
				//HAY QUE IMPRIMIR EN ORDEN CRECIENTE, HEAP CON LA OTRA FUNCION DE COMPARACION
				heap_encolar(heap, ip);
			}
			dos = 0;
			ult_impreso = ip;
		}

		if (difftime(*tiempo,*tiempo_aux) > 2){
			lista_iter_borrar(iter_ipfijo);
			lista_borrar_primero(lista_tiempo);
			leidos = getline (&linea,&cant,entrada);
			if (leidos != -1){ 
				// SPLIT HACE MALLOC
				// hago un split a linea
				char**registro = split(linea2,'\t');
				//inserto el ip en lista ip
				lista_insertar_ultimo(lista_ip, registro[0]);

				time_t* aux_tiemp = malloc(sizeof(time_t));
				*aux_tiemp = iso8601_to_time(registro[1]);
				lista_insertar_ultimo(lista_tiempo,aux_tiemp);
			}
			linea = NULL;
			lista_iter_destruir(iter_ip);
			lista_iter_t* iter_ip = lista_iter_crear(lista_ip);
			lista_iter_avanzar(iter_ip);

			lista_iter_destruir(iter_tiempo);
			lista_iter_t* iter_tiempo = lista_iter_crear(lista_tiempo);
			tiempo = lista_iter_ver_actual(iter_tiempo);
			lista_iter_avanzar(iter_tiempo);
			tiempo_aux = lista_iter_ver_actual(iter_tiempo);

		}
	}
	for (int i = 0; i < heap_cantidad(heap); i++){
		char*imprimir = heap_desencolar(heap); 
		printf("DoS: %s\n",imprimir);
	}	
	heap_destruir(heap,free);
	lista_iter_destruir(iter_ipfijo);
	lista_iter_destruir(iter_ip);
	lista_iter_destruir(iter_tiempo);
	lista_destruir(lista_ip,free);
	lista_destruir(lista_tiempo,free);
}

//la hago con la idea de que recibe un heap con los ips
void ver_vistitantes(char* desde, char* hasta, heap_t* heap){
	//ESTOY CREANDO UN heap_aux IGUAL A heap O SOLO ES UN PUNTERO?
	heap_t* heap_aux = heap;
	char*vector;
	//SPLIT HACE MALLOC
	char**dess = split(desde,'.');
	char**hass = split(hasta,'.');
	//join fue modificado para unir sin sep
	char*desj = join(dess);
	char*hasj = join(hass);
	size_t desde_num = atoi(desj);
	size_t hasta_num = atoi(hasj);

	char*ip = heap_desencolar(heap_aux);
	vector = ip;
	char**ips = split(hasta,'.');
	char*ipj = join(ips);
	size_t ip_num = atoi(ipj);

	printf("Visitantes:\n");
	while (!heap_esta_vacio(heap_aux)){ 
		if ((ip_num >= desde_num) && (ip_num <= hasta_num)){
			printf("\t%s\n",vector);
			ip = heap_desencolar(heap_aux);
			vector = ip;
			ips = split(hasta,'.');
			ipj = join(ips);
			ip_num = atoi(ipj);
		}
		if(ip_num < desde_num){
			ip = heap_desencolar(heap_aux);
			vector = ip;
			ips = split(hasta,'.');
			ipj = join(ips);
			ip_num = atoi(ipj);
		}	
		if(ip_num > hasta_num){
			break;
		}
	}
	heap_destruir(heap_aux,free);
}




int main(int argc, char*argv[]){

	heap_t* heap = heap_crear(cmp2);

	if (argc == 3) {
		if (strcmp(argv[1],"agregar_archivo") == 0){
			agregar_archivo(argv[2]);
			printf("OK\n");
			//abrir argv[2] sacar por split los ips y guardarlos en un heap
			FILE* archivo = fopen(argv[2],"r");
			size_t cant = 0;
			char*linea = NULL;
			ssize_t leidos = 0;
			while (leidos != -1){
				leidos = getline (&linea,&cant,archivo);
				if (leidos != -1){ 
					//SPLIT HACE MALLOC
					char**string = split(linea,'\t');
					heap_encolar(heap,string[0]);
				}
				linea = NULL;
			}
			fclose(archivo);
		}
		else{
			fprintf(stderr, "%s %s\n", "Error en comando", argv[1]);
			return 0;
		}
	}

	if (argc == 4){
		if (strcmp(argv[1],"ver_vistitantes") == 0){
			ver_vistitantes(argv[2],argv[3],heap);
			printf("OK\n");
		}
		else{
			fprintf(stderr, "%s %s\n", "Error en comando", argv[1]);
			return 0;
		}
	}
	if (argc == 5){
		if (strcmp(argv[1],"ordenar_archivo") == 0){
			size_t cap = atoi(argv[4]);
			ordenar_archivo(argv[2],argv[3],cap);
			printf("OK\n");
		}
		else{
			fprintf(stderr, "%s %s\n", "Error en comando", argv[1]);
			return 0;
		}
	}
	else{
		fprintf(stderr, "%s %s\n", "Error en comando", argv[1]);
		return 0;
	}
	
	heap_destruir(heap,free);
	return 0;
}