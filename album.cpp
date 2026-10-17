#include "Album.h"

Album::Album(const Texto& nombreAlbum, const Texto& ruta, char priv)
    : nombre(nombreAlbum), rutaAlmacenamiento(ruta), privacidad(priv)
{
}

Album::Album(const Album& otro)
    : nombre(otro.nombre), rutaAlmacenamiento(otro.rutaAlmacenamiento),
    privacidad(otro.privacidad)
{
    for (int i = 0; i < otro.fotos.getTamano(); ++i) {
        Medidor::contarIteracion();
        fotos.agregar(new Foto(*otro.fotos[i]));
    }
}

Album::~Album()
{
    liberarFotos();
}

void Album::liberarFotos()
{
    for (int i = 0; i < fotos.getTamano(); ++i) {
        Medidor::contarIteracion();
        delete fotos[i];
    }
}

Album& Album::operator=(const Album& otro)
{
    if (this != &otro) {
        liberarFotos();
        fotos = Lista<Foto*>();          // lista vacia
        nombre = otro.nombre;
        rutaAlmacenamiento = otro.rutaAlmacenamiento;
        privacidad = otro.privacidad;
        for (int i = 0; i < otro.fotos.getTamano(); ++i) {
            Medidor::contarIteracion();
            fotos.agregar(new Foto(*otro.fotos[i]));
        }
    }
    return *this;
}

const Texto& Album::getNombre() const { return nombre; }
const Texto& Album::getRuta() const   { return rutaAlmacenamiento; }
char Album::getPrivacidad() const     { return privacidad; }
int Album::getCantidadFotos() const   { return fotos.getTamano(); }
Foto* Album::getFoto(int indice) const { return fotos[indice]; }

void Album::agregarFoto(Foto* foto)
{
    fotos.agregar(foto);
}

bool Album::esVisible(bool esDueno, bool esAmigo) const
{
    if (esDueno) return true;
    if (privacidad == 'P') return true;
    if (privacidad == 'A') return esAmigo;
    return false;                        // 'R' privado
}

unsigned long Album::memoriaUsada() const
{
    unsigned long total = sizeof(*this) + fotos.memoriaDinamica()
    + memoriaDinamica(nombre)
        + memoriaDinamica(rutaAlmacenamiento);
    for (int i = 0; i < fotos.getTamano(); ++i) {
        Medidor::contarIteracion();
        total += fotos[i]->memoriaUsada();
    }
    return total;
}
