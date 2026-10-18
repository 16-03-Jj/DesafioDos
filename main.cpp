#include "Plataforma.h"
#include <new>
// Carpeta donde estan los .txt de datos. Si estan en el directorio de trabajo
// de la ejecucion se deja vacia; si no, escribir la ruta terminada en '/'.
// Ejemplo: "C:/Users/JUAN JOSE/Documents/Info2/datos/"
static const char* RUTA_DATOS = "C:/Users/JUAN JOSE/Documents/Info2/desafio 2/desafioDos/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/";

int main()
{
    // Todas las reservas con 'new' del programa pueden fallar con
    // std::bad_alloc; aqui se atrapa para terminar de forma controlada.
    try {
        Plataforma udeabook(RUTA_DATOS);
        if (!udeabook.cargarDatos())
            return 1;
        udeabook.ejecutar();
    } catch (const std::bad_alloc&) {
        std::cerr << "\nError: no hay memoria suficiente para continuar. "
                     "Se cierra el programa.\n";
        return 2;
    }
    return 0;
}