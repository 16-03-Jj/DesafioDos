#include "LectorLinea.h"
#include "Medidor.h"

LectorLinea::LectorLinea(const char* linea) : actual(linea) {}

void LectorLinea::limpiar(char* linea)
{
    int n = 0;
    while (linea[n] != '\0') {
        Medidor::contarIteracion();
        ++n;
    }
    if (n > 0 && linea[n - 1] == '\r')
        linea[n - 1] = '\0';
}

bool LectorLinea::terminado() const
{
    return *actual == '\0';
}

void LectorLinea::campo(char* destino, int maximo, char separador)
{
    int n = 0;
    while (*actual != '\0' && *actual != separador) {
        Medidor::contarIteracion();
        if (n < maximo - 1) destino[n++] = *actual;
        ++actual;
    }
    destino[n] = '\0';
    if (*actual == separador) ++actual;
}

int LectorLinea::entero(char separador)
{
    int valor = 0;
    while (*actual != '\0' && *actual != separador) {
        Medidor::contarIteracion();
        if (*actual >= '0' && *actual <= '9')
            valor = valor * 10 + (*actual - '0');
        ++actual;
    }
    if (*actual == separador) ++actual;
    return valor;
}

Fecha LectorLinea::fecha(char separador)
{
    char buf[16];
    campo(buf, 16, separador);
    short partes[3] = {0, 0, 0};
    int k = 0;
    for (int i = 0; buf[i] != '\0' && k < 3; ++i) {
        Medidor::contarIteracion();
        if (buf[i] == '/') ++k;
        else if (buf[i] >= '0' && buf[i] <= '9')
            partes[k] = static_cast<short>(partes[k] * 10 + (buf[i] - '0'));
    }
    return Fecha(partes[0], partes[1], partes[2]);
}

Texto LectorLinea::texto(char separador)
{
    char buf[1024];
    campo(buf, 1024, separador);
    return Texto(buf);
}

Texto LectorLinea::resto()
{
    Texto t(actual);
    while (*actual != '\0') {
        Medidor::contarIteracion();
        ++actual;
    }
    return t;
}