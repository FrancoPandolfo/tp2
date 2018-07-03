#include "pila.h"
#include <stdlib.h>

// ESTA COMENTADO EL FREE DATOS Y RENOMBRE DATOS A DATO.
/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'dato'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/
const size_t capacidad_inicial = 20; 
const size_t multiplicar_capacidad = 2;
const size_t porcentaje_capacidad_agrandar = 70;
const size_t dividir_capacidad = 2;
const size_t porcentaje_capacidad_achicar = 25;

pila_t* pila_crear(void){
	pila_t* pila = malloc (sizeof(pila_t));
	pila->datos = malloc (sizeof(void*));
	if ((pila == NULL) || (pila->datos == NULL)) return NULL;
	pila->cantidad = 0;
	pila->capacidad = capacidad_inicial;
	return pila;
}

size_t pila_cantidad(pila_t*pila){
	return pila->cantidad;
}

void pila_destruir (pila_t* pila){
	free(pila->datos);
	free(pila);
}

bool pila_redimensionar (pila_t* pila, size_t nuevo_tam){
	void** datos_nuevos;
	datos_nuevos = realloc(pila->datos,nuevo_tam * sizeof(void*));
	if (datos_nuevos == 0) return false;
	pila->datos=datos_nuevos;
	pila->capacidad=nuevo_tam;
	return true;	
}

bool pila_apilar (pila_t* pila,void* datos){
	if ((pila->cantidad == pila->capacidad) || (pila->cantidad ==  (pila->capacidad / 100) * porcentaje_capacidad_agrandar)) {
		pila_redimensionar(pila, pila->capacidad * multiplicar_capacidad);
	}
	if (pila->capacidad < pila->cantidad) return false;
	pila->datos[pila->cantidad] = datos;
	pila->cantidad++;
	return true;
}

bool pila_esta_vacia (const pila_t*pila){
	return pila->cantidad == 0;
}

void* pila_ver_tope (const pila_t* pila){
	if (pila_esta_vacia(pila)) return NULL;
	return pila->datos[pila->cantidad - 1];
}

void* pila_desapilar (pila_t* pila){
	if (pila_esta_vacia(pila)) return NULL;
	if ((pila->cantidad ==  (pila->capacidad / 100) * porcentaje_capacidad_achicar) && (pila->capacidad > capacidad_inicial)){
		pila_redimensionar(pila, pila->capacidad / dividir_capacidad);
	}
	void* valor = pila->datos[pila->cantidad - 1];
	pila->datos[pila->cantidad - 1] = NULL;
	pila->cantidad--;
	return valor;
}


// ...
