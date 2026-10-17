#include <iostream>
#include "Usuario.h"
#include "Medidor.h"

static Usuario crear(const char* nombre, short idx)
{
    return Usuario(nombre, "clave123", Fecha(1, 1, 2000), "correo@udea.edu.co",
                   "/users/X/perfil.png", "Medellin", "Colombia",
                   Fecha(5, 8, 2026), idx);
}

int main()
{
    Medidor::reiniciar();

    Usuario ana = crear("ana", 0);
    Usuario beto = crear("beto", 1);
    Usuario carlos = crear("carlos", 2);

    // --- operadores y contrasena ---
    std::cout << "ana == ana: " << (ana == crear("ana", 9)) << " (1), ana == beto: "
              << (ana == beto) << " (0)\n";
    std::cout << "ana < beto: " << (ana < beto) << " (1), beto < ana: "
              << (beto < ana) << " (0)\n";
    std::cout << "clave ok: " << ana.verificarContrasena("clave123") << " (1), mala: "
              << ana.verificarContrasena("otra") << " (0)\n";

    // --- mensajes: ana -> beto en 5 fechas ---
    short dias[5] = {1, 10, 15, 20, 30};
    for (int i = 0; i < 5; ++i) {
        Mensaje* m = new Mensaje(&ana, &beto, "hola beto", Fecha(dias[i], 9, 2026));
        ana.enviarMensaje(m);
        beto.recibirMensaje(m);
    }
    std::cout << "\nno leidos de beto: " << beto.cantidadNoLeidos() << " (5)\n";
    std::cout << *beto.getRecibido(0) << "\n";

    Lista<Mensaje*> r1;
    beto.mensajesEnRango(Fecha(10, 9, 2026), Fecha(20, 9, 2026), r1);
    std::cout << "rango 10-20/09: " << r1.getTamano() << " (3, extremos inclusive)\n";
    Lista<Mensaje*> r2;
    beto.mensajesEnRango(Fecha(11, 9, 2026), Fecha(14, 9, 2026), r2);
    std::cout << "rango 11-14/09: " << r2.getTamano() << " (0)\n";
    Lista<Mensaje*> r3;
    beto.mensajesEnRango(Fecha(1, 9, 2026), Fecha(30, 9, 2026), r3);
    std::cout << "rango 01-30/09: " << r3.getTamano() << " (5)\n";
    Lista<Mensaje*> r4;
    beto.mensajesEnRango(Fecha(1, 1, 2020), Fecha(1, 1, 2021), r4);
    std::cout << "rango antes de todo: " << r4.getTamano() << " (0)\n";
    Lista<Mensaje*> r5;
    beto.mensajesEnRango(Fecha(1, 12, 2026), Fecha(31, 12, 2026), r5);
    std::cout << "rango despues de todo: " << r5.getTamano() << " (0)\n";

    // --- lectura de nuevos ---
    beto.marcarNuevosComoLeidos();
    std::cout << "\ntras leer, no leidos: " << beto.cantidadNoLeidos() << " (0), "
              << "visto desde ana, enviado[0] leido: "
              << ana.getEnviado(0)->estaLeido() << " (1)\n";
    Mensaje* nuevo = new Mensaje(&ana, &beto, "mensaje nuevo", Fecha(2, 10, 2026));
    ana.enviarMensaje(nuevo);
    beto.recibirMensaje(nuevo);
    std::cout << "llega uno nuevo, no leidos: " << beto.cantidadNoLeidos() << " (1)\n";
    beto.recalcularPrimerNoLeido();
    std::cout << "recalcular (carga de datos): " << beto.cantidadNoLeidos() << " (1)\n";

    // --- solicitudes ---
    ana.agregarSolicitud(new Solicitud(&carlos, Fecha(3, 10, 2026)));
    std::cout << "\nana tiene solicitud de carlos(2): " << ana.tieneSolicitudDe(2)
              << " (1), de beto(1): " << ana.tieneSolicitudDe(1) << " (0)\n";
    ana.quitarSolicitud(0);
    std::cout << "solicitudes tras aceptar: " << ana.getCantidadSolicitudes() << " (0)\n";

    // --- albumes y copia profunda ---
    Album* alb = new Album("Cartagena2026", "/users/X/ana/albums/Cartagena2026", 'A');
    alb->agregarFoto(new Foto(Fecha(12, 6, 2026), 2048, "/users/X/ana/albums/Cartagena2026/m.png"));
    ana.agregarAlbum(alb);

    Usuario copia(ana);
    std::cout << "\ncopia: albumes = " << copia.getCantidadAlbumes() << " (1), enviados = "
              << copia.getCantidadEnviados() << " (6), album distinto: "
              << (copia.getAlbum(0) != ana.getAlbum(0)) << " (1)\n";
    Usuario asignado = crear("zeta", 7);
    asignado = ana;
    std::cout << "asignado: " << asignado.getNombreUsuario() << " (ana), albumes = "
              << asignado.getCantidadAlbumes() << " (1)\n";

    Medidor::registrarBytes(ana.memoriaUsada() + beto.memoriaUsada() + carlos.memoriaUsada());
    Medidor::mostrarReporte();
    return 0;
}