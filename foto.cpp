#include "Foto.h"
#include <iostream>

Foto::Foto(const Fecha& fecha, unsigned int tam, const Texto& ruta)
    : fechaCaptura(fecha), tamano(tam), rutaArchivo(ruta), likes(0)
{
}

Foto::Foto(const Foto& otra)
    : fechaCaptura(otra.fechaCaptura), tamano(otra.tamano),
    rutaArchivo(otra.rutaArchivo), likes(otra.likes),
    quienDioLike(otra.quienDioLike)
{
}

const Fecha& Foto::getFechaCaptura() const { return fechaCaptura; }
unsigned int Foto::getTamano() const       { return tamano; }
const Texto& Foto::getRuta() const         { return rutaArchivo; }
unsigned int Foto::getLikes() const        { return likes; }

int Foto::posicionLike(short idUsuario) const
{
    for (int i = 0; i < quienDioLike.getTamano(); ++i) {
        Medidor::contarIteracion();
        if (quienDioLike[i] == idUsuario)
            return i;
    }
    return -1;
}

bool Foto::yaDioLike(short idUsuario) const
{
    return posicionLike(idUsuario) >= 0;
}

bool Foto::alternarLike(short idUsuario)
{
    int pos = posicionLike(idUsuario);
    if (pos >= 0) {
        quienDioLike.eliminarSinOrden(pos);
        --likes;
        return false;
    }
    quienDioLike.agregar(idUsuario);
    ++likes;
    return true;
}

void Foto::mostrarRuta() const
{
    std::cout << rutaArchivo << "\n";
}

unsigned long Foto::memoriaUsada() const
{
    return sizeof(*this) + quienDioLike.memoriaDinamica()
    + memoriaDinamica(rutaArchivo);
}
