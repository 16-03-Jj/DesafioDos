#ifndef LECTORLINEA_H
#define LECTORLINEA_H

#include "Fecha.h"
#include "Texto.h"

// Recorre una linea de texto de un archivo de datos extrayendo campos
// separados por un caracter (por defecto ';'). Sin STL ni funciones de C.
class LectorLinea
{
private:
    const char* actual;

public:
    LectorLinea(const char* linea);

    // Elimina un '\r' final (archivos con fin de linea de Windows).
    static void limpiar(char* linea);

    bool terminado() const;
    void campo(char* destino, int maximo, char separador = ';');
    int entero(char separador = ';');
    Fecha fecha(char separador = ';');          // formato dd/mm/aaaa
    Texto texto(char separador = ';');
    Texto resto();                              // todo lo que queda de la linea
};

#endif // LECTORLINEA_H
