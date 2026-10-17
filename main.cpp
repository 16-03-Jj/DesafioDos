#include <iostream>
#include "Lista.h"
#include "Fecha.h"
#include "Medidor.h"

// Prueba rapida de los cimientos. Se reemplaza luego por el menu de UdeABook.
int main()
{
    Medidor::reiniciar();

    Lista<int> numeros;
    for (int i = 1; i <= 20; ++i)
        numeros.agregar(i * 10);

    Lista<int> copia(numeros);      // constructor de copia
    copia[0] = 999;                 // la copia es independiente

    std::cout << "original[0] = " << numeros[0]
              << ", copia[0] = " << copia[0]
              << ", tamano = " << numeros.getTamano() << "\n";

    Fecha a(15, 3, 2026), b(2, 10, 2026);
    std::cout << a << " < " << b << " ? " << (a < b) << "\n";
    std::cout << a << " <= " << a << " ? " << (a <= a) << "\n";
    std::cout << a << " == " << b << " ? " << (a == b) << "\n";

    Medidor::registrarBytes(numeros.memoriaUsada() + copia.memoriaUsada()
                            + 2 * sizeof(Fecha));
    Medidor::mostrarReporte();
    return 0;
}