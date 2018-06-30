#include "lista.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct nodo{
	void* dato;
	struct nodo* proximo;
}nodo_t;

struct lista{
	nodo_t* primero;
	size_t largo;
	nodo_t* ultimo;
};

struct lista_iter{
	nodo_t* actual;
	nodo_t* anterior;
	lista_t* lista;
};

void lista_iterar(lista_t*lista, bool visitar(void*dato,void*extra), void*extra){ 
	if((lista_esta_vacia(lista)) || (visitar == NULL)) return;
	nodo_t* auxnodo = lista->primero;
	while( (visitar(auxnodo->dato, extra)) && (auxnodo = auxnodo->proximo)){}
}

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	if (lista == NULL) return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;
	return lista;
}

nodo_t* nodo_crear(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(nodo == NULL)return NULL;
	nodo->dato = valor;
	nodo->proximo=NULL;
	return nodo;
}

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter == NULL) return NULL;
	iter->lista = lista;
	iter->actual = lista->primero;
	iter->anterior = NULL;
	return iter;	
}

void lista_destruir(lista_t*lista, void destruir_dato(void*)){
	while (!lista_esta_vacia(lista)){
		void*auxDato = lista_borrar_primero(lista);
		if (destruir_dato != NULL){ 
			destruir_dato(auxDato);
		}
	}
	free(lista);
}

bool lista_esta_vacia(const lista_t *lista){
	return lista->largo == 0;
}

bool lista_insertar_primero(lista_t *lista, void* valor){
	nodo_t* nodo = nodo_crear(valor);
	if (nodo == NULL)return false;
	if (lista_esta_vacia(lista)){
		lista->ultimo = nodo;
	}
	nodo->proximo = lista->primero;
	lista->primero = nodo;
	lista->largo++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void* valor){
	nodo_t* nodo = nodo_crear(valor);
	if (nodo == NULL)return false;
	if (lista_esta_vacia(lista)){
		lista->primero = nodo;
	}
	else{
		lista->ultimo->proximo = nodo;
	}
	lista->ultimo = nodo;
	lista->largo++;
	return true;
}

void* lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->primero->dato;
}

void* lista_ver_ultimo(const lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t*lista){
	return lista->largo;
}

void* lista_borrar_primero(lista_t*lista){
	if(lista_esta_vacia(lista)) return NULL;
	nodo_t* auxNodo = lista->primero;
	void* auxDato = auxNodo->dato;
	lista->primero = lista->primero->proximo;
	free(auxNodo);
	lista->largo--;
	return auxDato;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if(lista_iter_al_final(iter)) return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->proximo;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if(lista_iter_al_final(iter)) return NULL;
	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return (iter->actual == NULL);
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}


bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (nodo == NULL)return false;
	if (lista_iter_al_final(iter)){
		iter->lista->ultimo = nodo;
	}
	if (iter->anterior != NULL){ 
		iter->anterior->proximo = nodo;
	}
	else{
		iter->lista->primero = nodo;
	}
	nodo->proximo = iter->actual;
	iter->actual = nodo;
	iter->lista->largo++;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter){ 
	if (lista_iter_al_final(iter)) return NULL;
	nodo_t* nodo = iter->actual;
	void* dato = nodo->dato;
	iter->actual = nodo->proximo;
	if (nodo == iter->lista->ultimo){
		iter->lista->ultimo = iter->anterior;

	}
	else if (nodo == iter->lista->primero){
		iter->lista->primero = nodo->proximo;
	}
	else{ 
		iter->anterior->proximo = iter->actual;
	}
	iter->lista->largo--;
	free(nodo);
	return dato;
}

