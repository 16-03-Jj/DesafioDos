#include "Texto.h"
#include "Medidor.h"

int Texto::largo(const char* cadena)
{
    if (cadena == nullptr) return 0;
    int n = 0;
    while (cadena[n] != '\0') {
        Medidor::contarIteracion();
        ++n;
    }
    return n;
}

void Texto::reservar(int nuevaCapacidad)
{
    char* nuevo = new char[nuevaCapacidad];
    for (int i = 0; i <= longitud; ++i) {      // copia incluido el '\0'
        Medidor::contarIteracion();
        nuevo[i] = datos[i];
    }
    delete[] datos;
    datos = nuevo;
    capacidad = nuevaCapacidad;
}

Texto::Texto() : longitud(0), capacidad(1)
{
    datos = new char[1];
    datos[0] = '\0';
}

Texto::Texto(const char* cadena)
{
    longitud = largo(cadena);
    capacidad = longitud + 1;
    datos = new char[capacidad];
    for (int i = 0; i < longitud; ++i) {
        Medidor::contarIteracion();
        datos[i] = cadena[i];
    }
    datos[longitud] = '\0';
}

Texto::Texto(const Texto& otro)
    : longitud(otro.longitud), capacidad(otro.longitud + 1)
{
    datos = new char[capacidad];
    for (int i = 0; i <= longitud; ++i) {
        Medidor::contarIteracion();
        datos[i] = otro.datos[i];
    }
}

Texto::~Texto()
{
    delete[] datos;
}

Texto& Texto::operator=(const Texto& otro)
{
    if (this != &otro) {
        int nuevaCap = otro.longitud + 1;
        char* nuevo = new char[nuevaCap];
        for (int i = 0; i <= otro.longitud; ++i) {
            Medidor::contarIteracion();
            nuevo[i] = otro.datos[i];
        }
        delete[] datos;
        datos = nuevo;
        longitud = otro.longitud;
        capacidad = nuevaCap;
    }
    return *this;
}

Texto& Texto::operator=(const char* cadena)
{
    Texto temporal(cadena);
    return *this = temporal;
}

int Texto::size() const              { return longitud; }
bool Texto::vacio() const            { return longitud == 0; }
const char* Texto::c_str() const     { return datos; }
char Texto::operator[](int indice) const { return datos[indice]; }

bool Texto::operator==(const Texto& otro) const
{
    if (longitud != otro.longitud) return false;
    for (int i = 0; i < longitud; ++i) {
        Medidor::contarIteracion();
        if (datos[i] != otro.datos[i]) return false;
    }
    return true;
}

bool Texto::operator!=(const Texto& otro) const
{
    return !(*this == otro);
}

bool Texto::operator<(const Texto& otro) const
{
    int minimo = longitud < otro.longitud ? longitud : otro.longitud;
    for (int i = 0; i < minimo; ++i) {
        Medidor::contarIteracion();
        unsigned char a = static_cast<unsigned char>(datos[i]);
        unsigned char b = static_cast<unsigned char>(otro.datos[i]);
        if (a != b) return a < b;
    }
    return longitud < otro.longitud;       // el prefijo es menor
}

Texto& Texto::operator+=(const Texto& otro)
{
    if (this == &otro) {                   // t += t: se copia primero
        Texto copia(otro);
        return *this += copia;
    }
    int nuevaLongitud = longitud + otro.longitud;
    if (nuevaLongitud + 1 > capacidad) {
        int doble = capacidad * 2;
        reservar(doble > nuevaLongitud + 1 ? doble : nuevaLongitud + 1);
    }
    for (int i = 0; i <= otro.longitud; ++i) {   // incluye el '\0'
        Medidor::contarIteracion();
        datos[longitud + i] = otro.datos[i];
    }
    longitud = nuevaLongitud;
    return *this;
}

Texto Texto::operator+(const Texto& otro) const
{
    Texto resultado(*this);
    resultado += otro;
    return resultado;
}

std::ostream& operator<<(std::ostream& os, const Texto& t)
{
    return os << t.datos;
}

unsigned long Texto::memoriaDinamica() const
{
    return static_cast<unsigned long>(capacidad);
}

unsigned long Texto::memoriaUsada() const
{
    return sizeof(*this) + memoriaDinamica();
}