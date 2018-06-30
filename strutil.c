#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "strutil.h"

// largo total , el /0 reserva lugar para el sep
size_t strv_len(char**strv){
	int cantidad = 0;
	size_t len = 0;
	while(strv[cantidad] != NULL){
			len += strlen(strv[cantidad]);
			cantidad++;
	}
	return len;
}

// cantidad de substrings
int strv_cant(char**strv){
	int cantidad = 0;
	while(strv[cantidad] != NULL){
			cantidad++;
	}
	return (cantidad);
}


char** split(const char* str, char sep){
	if(sep == '\0')return NULL;
	int separador = 0;
	size_t largo_str = strlen(str);
	for(int i = 0; i < largo_str; i++){
		if(str[i] == sep){
			separador++;
		}
	}
	char**strv = malloc(sizeof(char*)*(separador + 2));
	if(strv == NULL)return NULL;
	//fijo el fin del arreglo de substrings.
	strv[separador + 1] = NULL;

	int numero_substr = 0;
	int largo_substr = 0;
	for(int i = 0; i < largo_str; i++){
		if(str[i] == sep){
			strv[numero_substr] = malloc(sizeof(char)*(largo_substr + 1));
			strncpy(strv[numero_substr], str + i - largo_substr, largo_substr );
			strv[numero_substr][largo_substr] = '\0';
			numero_substr++;
			largo_substr = 0;
		}
		else{ 
			largo_substr++;
		}
	}
	strv[numero_substr] = malloc(sizeof(char)*(largo_substr + 1));
	strcpy(strv[numero_substr], str + largo_str - largo_substr);
	return strv;
}

char* join(char** strv){
	//if(sep == '\0')return NULL;
	if(strv[0] == NULL){
		char*str = malloc(sizeof(char));
		if(str == NULL) return NULL;
		str[0] = '\0';
		return str;
	}
	int cant = strv_cant(strv);
	char*str = malloc(sizeof(char) * (strv_len(strv) + cant + 1) );
	if(str == NULL) return NULL;
	int cont = 0;
	size_t len = 0;
	for (int i = 0; i < cant ; i++){
		len = strlen(strv[i]);
		for (int j = 0; j < len; j++){
			str[cont] = strv[i][j];
			cont++;
		}
		//str[cont] = sep;
		cont++;
	}
	str[cont-1] = '\0';
	return str;
}

// libera un string de strings, liberando cada string y luego el string que los contiene.
void free_strv(char* strv[]){
	for (int i = 0; strv[i] != NULL; ++i){
		free(strv[i]);
	}
	free(strv);
} 