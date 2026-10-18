#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include <fstream>
#include <random>
#include "Lista.h"
#include "MatrizAmistad.h"
#include "Texto.h"
#include "Usuario.h"

class Plataforma
{
private:
    Lista<Usuario*> usuarios;      // ordenada por nombre de usuario
    MatrizAmistad* amistades;      // indexada por Usuario::indice
    Usuario* sesionActiva;
    Texto rutaDatos;               // carpeta de los archivos (con '/' final)
    short grupo[3];                // indices de los amigos cuya foto se muestra
    short cantidadGrupo;
    std::mt19937 generador;        // componente externo: <random>, C++17

    // --- carga de datos (funcionalidad I) ---
    bool abrirArchivo(std::ifstream& f, const char* nombre) const;
    bool cargarUsuarios();
    bool cargarAmistades();
    bool cargarAlbumes();
    bool cargarFotos();
    bool cargarSolicitudes();
    bool cargarMensajes();

    // --- guardado de datos (solo lo que el enunciado permite modificar) ---
    bool abrirSalida(std::ofstream& f, const char* nombre) const;
    bool guardarAmistades();
    bool guardarSolicitudes();
    bool guardarMensajes();
    bool guardarFotos();       // actualiza los likes

    int buscarUsuario(const Texto& nombre) const;   // -1 si no existe
    int aleatorio(int minimo, int maximo);          // usa <random> y lo reporta

    // Elige el siguiente grupo de 3 amigos (probabilidad ponderada).
    // Devuelve los bytes de variables locales usadas (para el reporte).
    unsigned long generarGrupo();
    void imprimirGrupo() const;

    // Valida y envia una solicitud de amistad al usuario 'destino'.
    // Imprime el resultado; devuelve true si se envio.
    bool enviarSolicitudA(int destino);
    void aceptarSolicitudes();

    // IV. Carrusel de fotos de un album (circular).
    void navegarAlbum(Album* album);

    // VI. Mensajes.
    void enviarMensaje();
    void leerMensajesNuevos();
    void buscarMensajesPorFecha();

    void reportarRecursos(unsigned long bytesLocales);
    void mostrarMenu() const;
    void menuPrincipal();

public:
    Plataforma(const Texto& carpetaDatos);
    Plataforma(const Plataforma&) = delete;
    Plataforma& operator=(const Plataforma&) = delete;
    ~Plataforma();

    bool cargarDatos();

    // Escribe amistades, solicitudes, mensajes y likes. Se llama al cerrar
    // sesion; no aparece en el menu.
    bool guardarDatos();
    bool iniciarSesion(const Texto& nombre, const Texto& clave);
    void cerrarSesion();

    // Funcionalidad II: 3 fotos de perfil de amigos, con prioridad por
    // amigos en comun. Genera un grupo nuevo, lo muestra y reporta recursos.
    void mostrarMejoresAmigos();

    // III. Enviar solicitudes y aceptar las recibidas.
    void administrarAmistades();

    // V. K sugerencias de amistad ordenadas por amigos en comun.
    void verSugerencias();

    // IV. Listar albumes visibles de un amigo y recorrer sus fotos.
    void visitarPerfilAmigo();

    // VI. Enviar, leer nuevos y buscar mensajes por rango de fechas.
    void administrarMensajes();

    // Ciclo principal: inicio de sesion y menu.
    void ejecutar();

    unsigned long memoriaUsada() const;
};

#endif // PLATAFORMA_H