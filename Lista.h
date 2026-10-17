#ifndef LISTA_H
#define LISTA_H

#include "Medidor.h"

// Arreglo dinamico generico de creacion propia (sin STL).
// Requisito sobre T: constructor por defecto y operador de asignacion.
// Para objetos grandes se recomienda almacenar punteros (Lista<Album*>).
template <typename T>
class Lista
{
private:
    T* datos;
    int tamano;
    int capacidad;

public:
    Lista(int capacidadInicial = 4);
    Lista(const Lista<T>& otra);
    ~Lista();

    Lista<T>& operator=(const Lista<T>& otra);

    void agregar(const T& elemento);

    // Sin verificacion de limites: el llamador debe usar 0 <= i < getTamano().
    T& operator[](int indice);
    const T& operator[](int indice) const;

    int getTamano() const;
    void redimensionar();

    // Elimina el elemento en 'indice' SIN conservar el orden: lo reemplaza
    // por el ultimo. Costo O(1).
    void eliminarSinOrden(int indice);

    // Bytes ocupados por la lista (objeto + bloque dinamico completo).
    // No incluye lo apuntado por los elementos si T es un puntero:
    // el dueno de esos objetos suma su propia memoria.
    unsigned long memoriaUsada() const;

    // Solo el bloque dinamico (capacidad * sizeof(T)). Lo usan las clases que
    // tienen una Lista como atributo, porque sizeof(*this) de ellas ya
    // incluye el objeto Lista.
    unsigned long memoriaDinamica() const;
};

template <typename T>
Lista<T>::Lista(int capacidadInicial)
    : tamano(0), capacidad(capacidadInicial > 0 ? capacidadInicial : 4)
{
    datos = new T[capacidad];
}

template <typename T>
Lista<T>::Lista(const Lista<T>& otra)
    : tamano(otra.tamano), capacidad(otra.capacidad)
{
    datos = new T[capacidad];
    for (int i = 0; i < tamano; ++i) {
        Medidor::contarIteracion();
        datos[i] = otra.datos[i];
    }
}

template <typename T>
Lista<T>::~Lista()
{
    delete[] datos;
}

template <typename T>
Lista<T>& Lista<T>::operator=(const Lista<T>& otra)
{
    if (this != &otra) {
        T* nuevo = new T[otra.capacidad];
        for (int i = 0; i < otra.tamano; ++i) {
            Medidor::contarIteracion();
            nuevo[i] = otra.datos[i];
        }
        delete[] datos;
        datos = nuevo;
        tamano = otra.tamano;
        capacidad = otra.capacidad;
    }
    return *this;
}

template <typename T>
void Lista<T>::agregar(const T& elemento)
{
    if (tamano == capacidad)
        redimensionar();
    datos[tamano++] = elemento;
}

template <typename T>
T& Lista<T>::operator[](int indice)
{
    return datos[indice];
}

template <typename T>
const T& Lista<T>::operator[](int indice) const
{
    return datos[indice];
}

template <typename T>
int Lista<T>::getTamano() const
{
    return tamano;
}

template <typename T>
void Lista<T>::redimensionar()
{
    int nuevaCapacidad = capacidad * 2;
    T* nuevo = new T[nuevaCapacidad];
    for (int i = 0; i < tamano; ++i) {
        Medidor::contarIteracion();
        nuevo[i] = datos[i];
    }
    delete[] datos;
    datos = nuevo;
    capacidad = nuevaCapacidad;
}

template <typename T>
void Lista<T>::eliminarSinOrden(int indice)
{
    datos[indice] = datos[tamano - 1];
    --tamano;
}

template <typename T>
unsigned long Lista<T>::memoriaDinamica() const
{
    return static_cast<unsigned long>(capacidad) * sizeof(T);
}

template <typename T>
unsigned long Lista<T>::memoriaUsada() const
{
    return sizeof(*this) + memoriaDinamica();
}

#endif // LISTA_H
