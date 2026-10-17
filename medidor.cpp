#include "Medidor.h"
#include <iostream>

unsigned long Medidor::iteraciones = 0;
unsigned long Medidor::bytesEnUso = 0;

void Medidor::reiniciar()
{
    iteraciones = 0;
    bytesEnUso = 0;
}

void Medidor::contarIteracion()
{
    ++iteraciones;
}

void Medidor::sumarIteraciones(unsigned long n)
{
    iteraciones += n;
}

void Medidor::registrarBytes(unsigned long bytes)
{
    bytesEnUso = bytes;
}

unsigned long Medidor::getIteraciones()
{
    return iteraciones;
}

unsigned long Medidor::getBytes()
{
    return bytesEnUso;
}

void Medidor::mostrarReporte()
{
    std::cout << "\n--- Consumo de recursos ---\n"
              << "Iteraciones: " << iteraciones << "\n"
              << "Memoria en uso: " << bytesEnUso << " bytes\n";
}