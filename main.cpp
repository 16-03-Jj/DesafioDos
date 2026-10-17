#include "Plataforma.h"

// Carpeta donde estan los .txt de datos. Si estan en el directorio de trabajo
// de la ejecucion se deja vacia; si no, escribir la ruta terminada en '/'.
// Ejemplo: "C:/Users/JUAN JOSE/Documents/Info2/datos/"
static const char* RUTA_DATOS = "C:/Users/JUAN JOSE/Documents/Info2/desafio 2/desafioDos/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/";

int main()
{
    Plataforma udeabook(RUTA_DATOS);
    if (!udeabook.cargarDatos())
        return 1;
    udeabook.ejecutar();
    return 0;
}