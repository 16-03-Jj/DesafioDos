#include "Medidor.h"
#include <iostream>

unsigned long Medidor::iteraciones = 0;
unsigned long Medidor::bytesEnUso = 0;
unsigned long Medidor::usosExternos = 0;
bool Medidor::contando = true;

void Medidor::reiniciar()
{
    iteraciones = 0;
    bytesEnUso = 0;
    usosExternos = 0;
    contando = true;
}

void Medidor::contarIteracion()
{
    if (contando) ++iteraciones;
}

void Medidor::sumarIteraciones(unsigned long n)
{
    if (contando) iteraciones += n;
}

void Medidor::registrarUsoExterno()
{
    if (contando) ++usosExternos;
}

void Medidor::pausar()   { contando = false; }
void Medidor::reanudar() { contando = true; }

void Medidor::registrarBytes(unsigned long bytes)
{
    bytesEnUso = bytes;
}

unsigned long Medidor::getIteraciones() { return iteraciones; }
unsigned long Medidor::getBytes()       { return bytesEnUso; }

void Medidor::mostrarReporte()
{
    std::cout << "\n--- Consumo de recursos ---\n"
              << "Iteraciones: " << iteraciones << "\n";
    if (usosExternos > 0)
        std::cout << "Invocaciones al generador de aleatorios (<random>): "
                  << usosExternos << "\n";
    std::cout << "Memoria en uso: " << bytesEnUso << " bytes\n";
}