#ifndef TEXTO_H
#define TEXTO_H

#include <iostream>

// Cadena de caracteres de creacion propia (sin std::string).
// Guarda los caracteres en un bloque dinamico terminado en '\0'.
class Texto
{
private:
    char* datos;
    int longitud;     // caracteres sin contar el '\0'
    int capacidad;    // bytes reservados (incluye el '\0')

    static int largo(const char* cadena);
    void reservar(int nuevaCapacidad);

public:
    Texto();
    Texto(const char* cadena);          // permite Texto t = "hola";
    Texto(const Texto& otro);
    ~Texto();

    Texto& operator=(const Texto& otro);
    Texto& operator=(const char* cadena);

    int size() const;
    bool vacio() const;
    const char* c_str() const;
    char operator[](int indice) const;  // sin verificacion de limites

    bool operator==(const Texto& otro) const;
    bool operator!=(const Texto& otro) const;
    bool operator<(const Texto& otro) const;   // orden lexicografico

    Texto& operator+=(const Texto& otro);
    Texto operator+(const Texto& otro) const;

    friend std::ostream& operator<<(std::ostream& os, const Texto& t);

    // Bytes del bloque dinamico (fuera del propio objeto).
    unsigned long memoriaDinamica() const;
    unsigned long memoriaUsada() const;
};

// Funcion libre usada por otras clases al medir su memoria.
inline unsigned long memoriaDinamica(const Texto& t)
{
    return t.memoriaDinamica();
}

#endif // TEXTO_H
