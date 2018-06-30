#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#define TAM_INICIAL 13

struct heap{
	void **datos;
	size_t cantidad;
	size_t capacidad;
	cmp_func_t cmp;
};

/* Funciones auxiliares */
void swap(void **vector,size_t pos, size_t pos_padre){
	void *aux = vector[pos];
	vector[pos] = vector[pos_padre];
	vector[pos_padre] = aux;
}

bool redimensionar(heap_t *heap, size_t tam_nuevo){
	void **datos_nuevos = realloc(heap->datos, tam_nuevo*sizeof(void *));
	if(!datos_nuevos) return false;
	heap->datos = datos_nuevos;
	heap->capacidad = 2*heap->capacidad;
	return true;
}

void upheap(void **vector, size_t pos, cmp_func_t cmp){
	if(pos == 0) return;
	size_t pos_padre = ((pos - 1) / 2);
	if(cmp(vector[pos], vector[pos_padre]) < 0) return;
	swap(vector, pos, pos_padre);
	upheap(vector, pos_padre, cmp);
}

void downheap(void** vector, size_t cant, size_t pos, cmp_func_t cmp){
	if (pos >= cant) return;
	size_t pos_izq = (2 * pos) + 1;
	size_t pos_der = (2 * pos) + 2;
	size_t pos_max = pos;
	if((pos_izq < cant) && (cmp(vector[pos_izq], vector[pos_max]) > 0)){
		pos_max = pos_izq;
	}
	if((pos_der < cant) && (cmp(vector[pos_der], vector[pos_max]) > 0)){
		pos_max = pos_der;
	}
	if(pos != pos_max){
		swap(vector, pos, pos_max);
		downheap(vector, cant, pos_max,cmp);
	}
}

void heapify(void **elementos, size_t cant, cmp_func_t cmp){
	for(size_t pos = cant; pos > 0; pos--){
		downheap(elementos, cant, pos - 1, cmp);
	}
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	heapify(elementos, cant, cmp);
	size_t largo_rel = cant - 1;
	for(size_t i = 0; i < cant; i++){
		swap(elementos, 0, largo_rel - i);
		downheap(elementos, largo_rel - i, 0, cmp);
	}
}

heap_t *heap_crear(cmp_func_t cmp){
	heap_t *heap = malloc(sizeof(heap_t));
	if(!heap) return NULL;
	heap->datos = malloc(TAM_INICIAL*sizeof(void *));
	if(!heap->datos){
		free(heap);
		return NULL;
	}
	heap->cantidad = 0;
	heap->capacidad = TAM_INICIAL;
	heap->cmp = cmp;
	return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
	heap_t *heap = heap_crear(cmp);
	if(!heap) return NULL;
	for(size_t i = 0; i < n; i++){
		heap_encolar(heap, arreglo[i]);
	}
	return heap;
}

bool heap_esta_vacio(const heap_t *heap){
	return heap->cantidad == 0;
}

size_t heap_cantidad(const heap_t *heap){
	return heap->cantidad;
}

void *heap_ver_max(const heap_t *heap){
	if(heap_esta_vacio(heap)) return NULL;
	return heap->datos[0];
}

bool heap_encolar(heap_t *heap, void *elem){
	heap->datos[heap->cantidad] = elem;
	heap->cantidad++;
	if(heap->cantidad > heap->capacidad / 2){
		if(!redimensionar(heap, 2*heap->capacidad)) return false;
	}
	upheap(heap->datos, heap->cantidad - 1, heap->cmp);
	return true;
}

void *heap_desencolar(heap_t *heap){
	if(heap_esta_vacio(heap)) return NULL;
	void* dato = heap->datos[0];
	swap(heap->datos,0,heap->cantidad - 1);
	heap->cantidad--;
	if(heap->cantidad > heap->capacidad / 4){
		redimensionar(heap, heap->capacidad / 2);
	}
	downheap(heap->datos, heap->cantidad, 0, heap->cmp);
	return dato;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	while (!heap_esta_vacio(heap)){
		void *ptr = heap_desencolar(heap);
		if(destruir_elemento) destruir_elemento(ptr);
	}
	free(heap->datos);
	free(heap);
}
