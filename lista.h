#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* defino el tipo lista */

typedef struct lista lista_t;

/* defino el tipo lista_iter */

typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void*));

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al comienzo
// de la lista.
bool lista_insertar_primero(lista_t *lista, void* valor);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void* valor);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void* lista_ver_ultimo(const lista_t *lista);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t*lista);

// obtiene el largo de la lista, cuantos elementos tiene, si la lista tiene
// elementos devuelve la cantidad, si esta vacia devuelve NULL.
// pre: la lista fue creada.
// post: se devolvio la cantidad de elementos, cuando no esta vacia.
size_t lista_largo(const lista_t*lista);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// defino el tipo iterador.
typedef struct lista_iter lista_iter_t;

// crea un iterador de lista.
// pre: la lista fue creada;
// post: devuelve un iterador.
lista_iter_t *lista_iter_crear(lista_t *lista);

// recibe un iterador y apunta al proximo elemento del iterador.
// pre: el iterador fue creado.
// post: se avanzo al proximo elemento de la lista iterada.
bool lista_iter_avanzar(lista_iter_t *iter);

// se obtiene el elemento al cual apunta el iterador.
// pre: el iterador fue creado.
// post: devuelve el elemento al cual apunta el iterador.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// el iterador apunta al final de la lista.
// pre: el iterador fue creado.
// post: el iterador ahora apunta el final de la lista. 
bool lista_iter_al_final(const lista_iter_t *iter);

// se destruye el iterador.
// pre: el iterador fue creado.
// post: se elimino el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// se inserta un elemento en donde apunta el iterador.
// pre: el iterador fue creado.
// post: se inserto un elemento donde apunta el iterador.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// se borra el elemento al cual apunta el iterador.
// pre: el iterador fue creado.
// post se borro el elemento donde apunta el iterador.
void *lista_iter_borrar(lista_iter_t *iter);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// itera una lista aplicando una funcion a cada dato mientras la funcion devuelva true, sino corta y no sigue iterando.
// pre: la lista fue creada.
// post: se aplico la funcion sobre la lista. 
void lista_iterar(lista_t*lista,bool visitar(void*dato,void*extra),void*extra);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);
#endif // 	LISTA_H
