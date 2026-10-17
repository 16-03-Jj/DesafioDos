#ifndef MATRIZAMISTAD_H
#define MATRIZAMISTAD_H

#include "Lista.h"

// Matriz de adyacencia de bits para la relacion de amistad (simetrica).
// Cada fila ocupa palabrasPorFila palabras de 64 bits: con 1000 usuarios
// la matriz completa pesa ~125 KB en lugar de 1 MB si fuera de bool.
class MatrizAmistad
{
private:
    unsigned long long* bits;
    short n;               // numero de usuarios
    short palabrasPorFila; // ceil(n / 64)

    static short contarBits(unsigned long long palabra); // popcount propio

public:
    MatrizAmistad(short numUsuarios);
    MatrizAmistad(const MatrizAmistad& otra);
    ~MatrizAmistad();
    MatrizAmistad& operator=(const MatrizAmistad& otra);

    // Registra la amistad en ambos sentidos.
    void agregarAmistad(short i, short j);

    bool sonAmigos(short i, short j) const;

    // Amigos en comun entre i y j: AND de las dos filas + conteo de bits.
    short comunes(short i, short j) const;

    short contarAmigos(short i) const;

    // Indices de los amigos de i.
    Lista<short> amigosDe(short i) const;

    unsigned long memoriaUsada() const;
};

#endif // MATRIZAMISTAD_H
