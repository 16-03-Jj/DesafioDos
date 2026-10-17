#include "Usuario.h"

Usuario::Usuario(const Texto& nombre, const Texto& clave, const Fecha& nacimiento,
                 const Texto& email, const Texto& rutaFoto, const Texto& ciudadU,
                 const Texto& paisU, const Fecha& inscripcion, short idx)
    : nombreUsuario(nombre), contrasena(clave), fechaNacimiento(nacimiento),
    correo(email), rutaFotoPerfil(rutaFoto), ciudad(ciudadU), pais(paisU),
    fechaInscripcion(inscripcion), indice(idx), primerNoLeido(0)
{
}

Usuario::Usuario(const Usuario& otro)
    : nombreUsuario(otro.nombreUsuario), contrasena(otro.contrasena),
    fechaNacimiento(otro.fechaNacimiento), correo(otro.correo),
    rutaFotoPerfil(otro.rutaFotoPerfil), ciudad(otro.ciudad), pais(otro.pais),
    fechaInscripcion(otro.fechaInscripcion), indice(otro.indice),
    primerNoLeido(otro.primerNoLeido)
{
    copiarDuenos(otro);
}

Usuario::~Usuario()
{
    liberarDuenos();
}

void Usuario::copiarDuenos(const Usuario& otro)
{
    for (int i = 0; i < otro.albumes.getTamano(); ++i) {
        Medidor::contarIteracion();
        albumes.agregar(new Album(*otro.albumes[i]));
    }
    for (int i = 0; i < otro.enviados.getTamano(); ++i) {
        Medidor::contarIteracion();
        enviados.agregar(new Mensaje(*otro.enviados[i]));
    }
    for (int i = 0; i < otro.recibidos.getTamano(); ++i) {
        Medidor::contarIteracion();
        recibidos.agregar(otro.recibidos[i]);       // puntero compartido
    }
    for (int i = 0; i < otro.solicitudes.getTamano(); ++i) {
        Medidor::contarIteracion();
        solicitudes.agregar(new Solicitud(*otro.solicitudes[i]));
    }
}

void Usuario::liberarDuenos()
{
    for (int i = 0; i < albumes.getTamano(); ++i) {
        Medidor::contarIteracion();
        delete albumes[i];
    }
    for (int i = 0; i < enviados.getTamano(); ++i) {
        Medidor::contarIteracion();
        delete enviados[i];
    }
    for (int i = 0; i < solicitudes.getTamano(); ++i) {
        Medidor::contarIteracion();
        delete solicitudes[i];
    }
}

Usuario& Usuario::operator=(const Usuario& otro)
{
    if (this != &otro) {
        liberarDuenos();
        albumes = Lista<Album*>();
        enviados = Lista<Mensaje*>();
        recibidos = Lista<Mensaje*>();
        solicitudes = Lista<Solicitud*>();

        nombreUsuario = otro.nombreUsuario;
        contrasena = otro.contrasena;
        fechaNacimiento = otro.fechaNacimiento;
        correo = otro.correo;
        rutaFotoPerfil = otro.rutaFotoPerfil;
        ciudad = otro.ciudad;
        pais = otro.pais;
        fechaInscripcion = otro.fechaInscripcion;
        indice = otro.indice;
        primerNoLeido = otro.primerNoLeido;
        copiarDuenos(otro);
    }
    return *this;
}

bool Usuario::operator==(const Usuario& otro) const
{
    return nombreUsuario == otro.nombreUsuario;
}

bool Usuario::operator<(const Usuario& otro) const
{
    return nombreUsuario < otro.nombreUsuario;
}

const Texto& Usuario::getNombreUsuario() const   { return nombreUsuario; }
const Texto& Usuario::getCorreo() const          { return correo; }
const Texto& Usuario::getRutaFotoPerfil() const  { return rutaFotoPerfil; }
const Texto& Usuario::getCiudad() const          { return ciudad; }
const Texto& Usuario::getPais() const            { return pais; }
const Fecha& Usuario::getFechaNacimiento() const { return fechaNacimiento; }
const Fecha& Usuario::getFechaInscripcion() const { return fechaInscripcion; }
short Usuario::getIndice() const                 { return indice; }

bool Usuario::verificarContrasena(const Texto& clave) const
{
    return contrasena == clave;
}

// ---------------- albumes ----------------
void Usuario::agregarAlbum(Album* album)  { albumes.agregar(album); }
int Usuario::getCantidadAlbumes() const   { return albumes.getTamano(); }
Album* Usuario::getAlbum(int i) const     { return albumes[i]; }

// ---------------- mensajes ----------------
void Usuario::enviarMensaje(Mensaje* mensaje)  { enviados.agregar(mensaje); }
void Usuario::recibirMensaje(Mensaje* mensaje) { recibidos.agregar(mensaje); }
int Usuario::getCantidadEnviados() const       { return enviados.getTamano(); }
Mensaje* Usuario::getEnviado(int i) const      { return enviados[i]; }
int Usuario::getCantidadRecibidos() const      { return recibidos.getTamano(); }
Mensaje* Usuario::getRecibido(int i) const     { return recibidos[i]; }
int Usuario::getPrimerNoLeido() const          { return primerNoLeido; }

int Usuario::cantidadNoLeidos() const
{
    return recibidos.getTamano() - primerNoLeido;
}

void Usuario::marcarNuevosComoLeidos()
{
    for (int i = primerNoLeido; i < recibidos.getTamano(); ++i) {
        Medidor::contarIteracion();
        recibidos[i]->marcarLeido();
    }
    primerNoLeido = recibidos.getTamano();
}

void Usuario::recalcularPrimerNoLeido()
{
    int i = recibidos.getTamano();
    while (i > 0 && !recibidos[i - 1]->estaLeido()) {
        Medidor::contarIteracion();
        --i;
    }
    primerNoLeido = i;
}

void Usuario::mensajesEnRango(const Fecha& desde, const Fecha& hasta,
                              Lista<Mensaje*>& resultado) const
{
    int n = recibidos.getTamano();

    // 1) primer indice con fecha >= desde
    int ini = 0, fin = n;
    while (ini < fin) {
        Medidor::contarIteracion();
        int medio = ini + (fin - ini) / 2;
        if (recibidos[medio]->getFechaEnvio() < desde) ini = medio + 1;
        else fin = medio;
    }
    int primero = ini;

    // 2) primer indice con fecha > hasta
    fin = n;
    while (ini < fin) {
        Medidor::contarIteracion();
        int medio = ini + (fin - ini) / 2;
        if (recibidos[medio]->getFechaEnvio() <= hasta) ini = medio + 1;
        else fin = medio;
    }
    int limite = ini;

    // 3) copiar los punteros del rango [primero, limite)
    for (int i = primero; i < limite; ++i) {
        Medidor::contarIteracion();
        resultado.agregar(recibidos[i]);
    }
}

// ---------------- solicitudes ----------------
void Usuario::agregarSolicitud(Solicitud* solicitud) { solicitudes.agregar(solicitud); }
int Usuario::getCantidadSolicitudes() const          { return solicitudes.getTamano(); }
Solicitud* Usuario::getSolicitud(int i) const        { return solicitudes[i]; }

bool Usuario::tieneSolicitudDe(short indiceEmisor) const
{
    for (int i = 0; i < solicitudes.getTamano(); ++i) {
        Medidor::contarIteracion();
        if (solicitudes[i]->getEmisor()->getIndice() == indiceEmisor)
            return true;
    }
    return false;
}

void Usuario::quitarSolicitud(int i)
{
    delete solicitudes[i];
    solicitudes.eliminarSinOrden(i);
}

// ---------------- memoria ----------------
unsigned long Usuario::memoriaUsada() const
{
    unsigned long total = sizeof(*this)
    + memoriaDinamica(nombreUsuario) + memoriaDinamica(contrasena)
        + memoriaDinamica(correo) + memoriaDinamica(rutaFotoPerfil)
        + memoriaDinamica(ciudad) + memoriaDinamica(pais)
        + albumes.memoriaDinamica() + enviados.memoriaDinamica()
        + recibidos.memoriaDinamica() + solicitudes.memoriaDinamica();

    for (int i = 0; i < albumes.getTamano(); ++i) {
        Medidor::contarIteracion();
        total += albumes[i]->memoriaUsada();
    }
    for (int i = 0; i < enviados.getTamano(); ++i) {      // los recibidos NO
        Medidor::contarIteracion();
        total += enviados[i]->memoriaUsada();
    }
    for (int i = 0; i < solicitudes.getTamano(); ++i) {
        Medidor::contarIteracion();
        total += solicitudes[i]->memoriaUsada();
    }
    return total;
}
