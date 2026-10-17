#ifndef FOTO_H
#define FOTO_H

#include "Fecha.h"
#include "Lista.h"
#include "Texto.h"

class Foto
{
private:
    Fecha fechaCaptura;
    unsigned int tamano;
    Texto rutaArchivo;
    unsigned int likes;
    Lista<short> quienDioLike;   // indices de los usuarios que dieron like

    int posicionLike(short idUsuario) const;   // -1 si no ha dado like

public:
    Foto(const Fecha& fecha, unsigned int tam, const Texto& ruta);
    Foto(const Foto& otra);

    const Fecha& getFechaCaptura() const;
    unsigned int getTamano() const;
    const Texto& getRuta() const;
    unsigned int getLikes() const;
    bool yaDioLike(short idUsuario) const;

    // Da like si el usuario no lo habia dado; lo quita si ya lo habia dado.
    // Devuelve true si quedo con like, false si se quito.
    bool alternarLike(short idUsuario);

    // Visualizar una foto = mostrar la ruta de su archivo.
    void mostrarRuta() const;

    unsigned long memoriaUsada() const;
};

#endif // FOTO_H