#ifndef ALBUM_H
#define ALBUM_H

#include "Foto.h"
#include "Lista.h"
#include "Texto.h"

// Codigos de privacidad (atributo char):
//   'P' = publico   (lo ve cualquier usuario)
//   'R' = privado   (solo el dueno)
//   'A' = amigos    (el dueno y sus amigos)
class Album
{
private:
    Texto nombre;
    Texto rutaAlmacenamiento;
    char privacidad;
    Lista<Foto*> fotos;          // el album es DUENO de sus fotos

    void liberarFotos();

public:
    Album(const Texto& nombreAlbum, const Texto& ruta, char priv);
    Album(const Album& otro);              // copia profunda
    ~Album();
    Album& operator=(const Album& otro);   // copia profunda

    const Texto& getNombre() const;
    const Texto& getRuta() const;
    char getPrivacidad() const;
    int getCantidadFotos() const;
    Foto* getFoto(int indice) const;

    // El album toma posesion del puntero (lo libera en su destructor).
    void agregarFoto(Foto* foto);

    // esDueno: quien consulta es el propietario. esAmigo: es amigo del dueno.
    bool esVisible(bool esDueno, bool esAmigo) const;

    unsigned long memoriaUsada() const;
};

#endif // ALBUM_H
