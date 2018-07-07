#define _DEFAULT_SOURCE
#include "lista.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#define HASHSIZE 33
#define FACTOR_CARGA 2

/* Definiciones auxiliares */
typedef struct campo_hash{
	const char *clave;
	void *dato;
}campo_hash_t;

campo_hash_t *campo_hash_crear(const char *clave, void *dato ){
	campo_hash_t *campo_hash = malloc(sizeof(campo_hash_t));
	if(!campo_hash) return NULL;
	campo_hash->clave = strdup(clave);
	campo_hash->dato = dato;
	return campo_hash;
}
/* Primitivas del hash */

struct hash{
	lista_t **tabla;
	size_t cantidad;
	size_t capacidad;
	hash_destruir_dato_t hash_destruir_dato;
};

/* Función de hashing */
size_t hashing(const char* clave, size_t tam){
    size_t hash = 5381;
    int c;

    while((c = *clave++) != 0){
        hash = ((hash << 5) + hash) + c;
    }
    return hash % tam;
}

hash_t *hash_crear(hash_destruir_dato_t hash_destruir_dato){
	hash_t *hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;
	hash->tabla = malloc(HASHSIZE*sizeof(lista_t *));
	if(!hash->tabla){
		free(hash);
		return NULL;
	}
	hash->cantidad = 0;
	hash->capacidad = HASHSIZE;
	hash->hash_destruir_dato = hash_destruir_dato;
	for(size_t i = 0; i < hash->capacidad; i++){
		hash->tabla[i] = lista_crear(); 
	}
	return hash;
}

/*                         Hash Guardar                             */
/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/
void rehashing(hash_t *hash){
	lista_t **tabla_nueva = malloc((2*(hash->capacidad) + 1)*sizeof(lista_t *));
	if(!tabla_nueva) return;
	for(size_t i = 0; i < 2*hash->capacidad + 1; i++){
		tabla_nueva[i] = lista_crear();
	}
	for(size_t i = 0; i < hash->capacidad; i++){
		while(!lista_esta_vacia(hash->tabla[i])){
			campo_hash_t *campo = lista_borrar_primero(hash->tabla[i]);
			if(campo){
				size_t pos = hashing(campo->clave, 2*hash->capacidad + 1);
				lista_insertar_ultimo(tabla_nueva[pos], campo);
			}
		}
		lista_destruir(hash->tabla[i], hash->hash_destruir_dato);
	}
	free(hash->tabla);
	hash->tabla = tabla_nueva;
	hash->capacidad = 2*hash->capacidad + 1;
}

bool clave_encontrada(const char *clave, void *campo){
	if(!campo) return false;
	return strcmp(((campo_hash_t *)campo)->clave, clave) == 0;
}

campo_hash_t *campo_hash_buscar(lista_iter_t *iter, const char *clave){
	while(!lista_iter_al_final(iter)){
		campo_hash_t *campo = lista_iter_ver_actual(iter);
		if(clave_encontrada(clave, campo)) return campo;
		lista_iter_avanzar(iter);
	}
	return NULL;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	size_t pos = hashing(clave, hash->capacidad);
	lista_iter_t *iter = lista_iter_crear(hash->tabla[pos]);
	if(!campo_hash_buscar(iter, clave)){
		campo_hash_t *campo = campo_hash_crear(clave, dato);
		if(!campo){
			lista_iter_destruir(iter);
			return false;
		}
		while(!lista_iter_al_final(iter)){
			lista_iter_avanzar(iter);
		}
		lista_iter_insertar(iter, campo);
		hash->cantidad++;
	}else{;
		campo_hash_t *campo = lista_iter_ver_actual(iter);
		if(hash->hash_destruir_dato!=NULL){
			hash->hash_destruir_dato(campo->dato);
		} 
		campo->dato = dato;
	}
	if(hash->cantidad / hash->capacidad == FACTOR_CARGA){
		rehashing(hash);
	}
	lista_iter_destruir(iter);
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	size_t pos = hashing(clave, hash->capacidad);
	lista_iter_t *iter = lista_iter_crear(hash->tabla[pos]);
	void *dato;
	if(!campo_hash_buscar(iter, clave)){
		dato = NULL;
	}else{
		campo_hash_t *campo = (campo_hash_t *)lista_iter_borrar(iter);
		dato = campo->dato;
		free((void *)campo->clave);
		free(campo);
		hash->cantidad--;
	}
	lista_iter_destruir(iter);
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = hashing(clave, hash->capacidad);
	lista_iter_t *iter = lista_iter_crear(hash->tabla[pos]);
	void *dato;
	if(!campo_hash_buscar(iter, clave)){
		dato = NULL;
	}else{
		dato = ((campo_hash_t *)lista_iter_ver_actual(iter))->dato;
	}
	lista_iter_destruir(iter);
	return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = hashing(clave, hash->capacidad);
	lista_iter_t *iter = lista_iter_crear(hash->tabla[pos]);
	bool ok = campo_hash_buscar(iter, clave);
	lista_iter_destruir(iter);
	return ok;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

void lista_limpiar(lista_t *lista, hash_destruir_dato_t hash_destruir_dato){
	while(!lista_esta_vacia(lista)){
		campo_hash_t *campo = lista_borrar_primero(lista);
		free((void *)campo->clave);
		if(hash_destruir_dato) hash_destruir_dato(campo->dato);
		free(campo);
	}
	free(lista);
}

void hash_destruir(hash_t *hash){
	for(size_t i = 0; i < hash->capacidad; i++){
		lista_limpiar(hash->tabla[i], hash->hash_destruir_dato);
	}
	free(hash->tabla);
	free(hash);
}

/* Primitivas para el iterador del hash */
struct hash_iter{
	size_t pos;
	const hash_t *hash;
	lista_iter_t *lista_iter;
};

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t *iter = malloc(sizeof(hash_iter_t));
	if(!iter) return NULL;
	iter->hash = hash;
	iter->pos = 0;
	while(lista_esta_vacia(hash->tabla[iter->pos]) && iter->pos < hash->capacidad - 1){
		iter->pos++;
	}
	iter->lista_iter = lista_iter_crear(hash->tabla[iter->pos]);
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(!lista_iter_al_final(iter->lista_iter)) lista_iter_avanzar(iter->lista_iter);
	if(lista_iter_al_final(iter->lista_iter) && iter->pos < iter->hash->capacidad - 1){
		lista_iter_destruir(iter->lista_iter);
		do{
			iter->pos++;
		}while(lista_esta_vacia(iter->hash->tabla[iter->pos]) && iter->pos < iter->hash->capacidad - 1);
		iter->lista_iter = lista_iter_crear(iter->hash->tabla[iter->pos]);
	}
	if(hash_iter_al_final(iter)) return false;
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)) return NULL;
	return ((campo_hash_t *)lista_iter_ver_actual(iter->lista_iter))->clave;
}

void *hash_iter_ver_actual_dato(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)) return NULL;
	return ((campo_hash_t *)lista_iter_ver_actual(iter->lista_iter))->dato;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return (iter->pos == iter->hash->capacidad - 1) && lista_iter_al_final(iter->lista_iter);
}

void hash_iter_destruir(hash_iter_t *iter){
	lista_iter_destruir(iter->lista_iter);
	free(iter);
}