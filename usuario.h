#ifndef USUARIO_H
#define USUARIO_H

#include "Album.h"
#include "Fecha.h"
#include "Lista.h"
#include "Mensaje.h"
#include "Solicitud.h"
#include "Texto.h"

// Propiedad de los objetos (quien los libera):
//   albumes, enviados, solicitudes -> el Usuario es DUENO.
//   recibidos -> solo punteros a mensajes que pertenecen al emisor.
// La amistad NO se guarda aqui: vive en MatrizAmistad (indexada por 'indice').
class Usuario
{
private:
    Texto nombreUsuario;
    Texto contrasena;
    Fecha fechaNacimiento;
    Texto correo;
    Texto rutaFotoPerfil;
    Texto ciudad;
    Texto pais;
    Fecha fechaInscripcion;
    short indice;                    // posicion en Plataforma y en la matriz

    Lista<Album*> albumes;
    Lista<Mensaje*> enviados;
    Lista<Mensaje*> recibidos;       // en orden cronologico de llegada
    int primerNoLeido;               // recibidos[primerNoLeido..fin) no leidos
    Lista<Solicitud*> solicitudes;   // pendientes recibidas

    void copiarDuenos(const Usuario& otro);
    void liberarDuenos();

public:
    Usuario(const Texto& nombre, const Texto& clave, const Fecha& nacimiento,
            const Texto& email, const Texto& rutaFoto, const Texto& ciudadU,
            const Texto& paisU, const Fecha& inscripcion, short idx);
    Usuario(const Usuario& otro);             // copia profunda de lo que posee
    ~Usuario();
    Usuario& operator=(const Usuario& otro);

    bool operator==(const Usuario& otro) const;   // mismo nombreUsuario
    bool operator<(const Usuario& otro) const;    // orden por nombreUsuario

    // --- datos personales ---
    const Texto& getNombreUsuario() const;
    const Texto& getCorreo() const;
    const Texto& getRutaFotoPerfil() const;
    const Texto& getCiudad() const;
    const Texto& getPais() const;
    const Fecha& getFechaNacimiento() const;
    const Fecha& getFechaInscripcion() const;
    short getIndice() const;
    bool verificarContrasena(const Texto& clave) const;

    // --- albumes ---
    void agregarAlbum(Album* album);              // toma posesion
    int getCantidadAlbumes() const;
    Album* getAlbum(int i) const;

    // --- mensajes ---
    void enviarMensaje(Mensaje* mensaje);         // toma posesion
    void recibirMensaje(Mensaje* mensaje);        // solo guarda el puntero
    int getCantidadEnviados() const;
    Mensaje* getEnviado(int i) const;
    int getCantidadRecibidos() const;
    Mensaje* getRecibido(int i) const;

    int cantidadNoLeidos() const;
    int getPrimerNoLeido() const;
    void marcarNuevosComoLeidos();                // marca tambien en el emisor
    void recalcularPrimerNoLeido();               // usar despues de cargar datos

    // Mensajes recibidos con desde <= fecha <= hasta (busqueda binaria).
    // Requiere 'recibidos' ordenado cronologicamente.
    void mensajesEnRango(const Fecha& desde, const Fecha& hasta,
                         Lista<Mensaje*>& resultado) const;

    // --- solicitudes ---
    void agregarSolicitud(Solicitud* solicitud);  // toma posesion
    bool tieneSolicitudDe(short indiceEmisor) const;
    int getCantidadSolicitudes() const;
    Solicitud* getSolicitud(int i) const;
    void quitarSolicitud(int i);                  // la libera (no conserva orden)

    unsigned long memoriaUsada() const;
};

#endif // USUARIO_H
