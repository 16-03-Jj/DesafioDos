#include <iostream>
#include "Texto.h"
#include "Medidor.h"

int main()
{
    Medidor::reiniciar();

    Texto a("hola");
    Texto b = "mundo";                       // conversion desde literal
    Texto c = a + " " + b;
    std::cout << "concatenar: " << c << " (esperado: hola mundo), size = "
              << c.size() << " (esperado 10)\n";

    Texto d(c);                              // copia independiente
    d += "!";
    std::cout << "original: " << c << " | copia: " << d
              << " (esperado: hola mundo | hola mundo!)\n";

    Texto e;
    std::cout << "vacio: " << e.vacio() << " (esperado 1)\n";
    e = d;                                   // asignacion
    e = e;                                   // autoasignacion segura
    std::cout << "asignado: " << e << " (esperado: hola mundo!)\n";

    Texto f("ab");
    f += f;                                  // autoconcatenacion
    std::cout << "t += t: " << f << " (esperado: abab)\n";

    std::cout << "== : " << (Texto("juan") == Texto("juan")) << " (1), "
              << (Texto("juan") == Texto("juana")) << " (0)\n";
    std::cout << "<  : " << (Texto("ana") < Texto("beto")) << " (1), "
              << (Texto("beto") < Texto("ana")) << " (0), "
              << (Texto("ana") < Texto("anabel")) << " (1: prefijo), "
              << (Texto("ana") < Texto("ana")) << " (0)\n";
    std::cout << "[1] de 'hola' = " << a[1] << " (esperado o)\n";

    Medidor::registrarBytes(a.memoriaUsada() + b.memoriaUsada() + c.memoriaUsada()
                            + d.memoriaUsada() + e.memoriaUsada() + f.memoriaUsada());
    Medidor::mostrarReporte();
    return 0;
}