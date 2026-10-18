#include "Plataforma.h"
#include "LectorLinea.h"
#include <chrono>
#include <iostream>

static const int MAX_LINEA = 1024;

// Tras el ciclo de lectura el flujo debe haber llegado al final del archivo.
// Si no, una linea excedio el buffer o el archivo es ilegible: no se continua
// en silencio con datos incompletos.
static bool lecturaCompleta(const std::ifstream& f, const char* archivo)
{
    if (f.eof()) return true;
    std::cerr << "Error: linea demasiado larga o ilegible en " << archivo << "\n";
    return false;
}

// Lee una linea larga (mensajes). Devuelve 0 = ok, 1 = demasiado larga,
// -1 = se cerro la entrada.
static int leerLineaLarga(char* buffer, int maximo)
{
    std::cin.getline(buffer, maximo);
    if (!std::cin) {
        if (std::cin.eof()) { buffer[0] = '\0'; return -1; }
        std::cin.clear();                       // linea mas larga que el buffer
        std::cin.ignore(1000000, '\n');
        return 1;
    }
    LectorLinea::limpiar(buffer);
    return 0;
}

// Convierte "dd/mm/aaaa" en una Fecha valida. false si el formato falla.
static bool leerFecha(const char* texto, Fecha& resultado)
{
    int partes[3] = {0, 0, 0};
    int k = 0, digitos = 0;
    for (int i = 0; texto[i] != '\0'; ++i) {
        Medidor::contarIteracion();
        char c = texto[i];
        if (c >= '0' && c <= '9') {
            partes[k] = partes[k] * 10 + (c - '0');
            ++digitos;
            if (partes[k] > 9999) return false;
        } else if (c == '/') {
            if (digitos == 0 || k == 2) return false;
            ++k;
            digitos = 0;
        } else if (c != ' ') {
            return false;
        }
    }
    if (k != 2 || digitos == 0) return false;
    Fecha f(static_cast<short>(partes[0]), static_cast<short>(partes[1]),
            static_cast<short>(partes[2]));
    if (!f.esValida()) return false;
    resultado = f;
    return true;
}

// Lee una linea de la consola. Devuelve false si se cerro la entrada.
static bool leerLinea(char* buffer, int maximo)
{
    std::cin.getline(buffer, maximo);
    if (!std::cin) {
        if (std::cin.eof()) { buffer[0] = '\0'; return false; }
        std::cin.clear();                    // linea mas larga que el buffer:
        std::cin.ignore(1000000, '\n');      // se descarta lo que sobra
    }
    LectorLinea::limpiar(buffer);
    return true;
}

// Convierte la linea COMPLETA en un entero >= 0. false si esta vacia, tiene
// algo que no sea digito (letras, signo menos...) o el numero es enorme.
static bool leerEnteroEstricto(const char* texto, int& valor)
{
    int i = 0;
    while (texto[i] == ' ') ++i;
    if (texto[i] == '\0') return false;
    long long acumulado = 0;
    while (texto[i] >= '0' && texto[i] <= '9') {
        Medidor::contarIteracion();
        acumulado = acumulado * 10 + (texto[i] - '0');
        if (acumulado > 1000000000LL) return false;
        ++i;
    }
    while (texto[i] == ' ') ++i;
    if (texto[i] != '\0') return false;
    valor = static_cast<int>(acumulado);
    return true;
}

// Una opcion de menu es valida solo si es UN caracter ("12" no cuenta como "1").
static char opcion(const char* linea)
{
    return (linea[0] != '\0' && linea[1] == '\0') ? linea[0] : '?';
}

Plataforma::Plataforma(const Texto& carpetaDatos)
    : amistades(nullptr), sesionActiva(nullptr), rutaDatos(carpetaDatos),
    cantidadGrupo(0),
    generador(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()))
{
    grupo[0] = grupo[1] = grupo[2] = -1;
}

Plataforma::~Plataforma()
{
    for (int i = 0; i < usuarios.getTamano(); ++i)
        delete usuarios[i];
    delete amistades;
}

// =====================================================================
//  I. Carga de datos
// =====================================================================
bool Plataforma::abrirArchivo(std::ifstream& f, const char* nombre) const
{
    Texto ruta = rutaDatos + nombre;
    f.open(ruta.c_str());
    if (!f) {
        std::cerr << "Error: no se pudo abrir " << ruta << "\n";
        return false;
    }
    return true;
}

bool Plataforma::cargarUsuarios()
{
    std::ifstream f;
    if (!abrirArchivo(f, "usuarios.txt")) return false;

    char linea[MAX_LINEA];
    short idx = 0;
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;

        LectorLinea l(linea);
        Texto nombre = l.texto();
        Texto clave = l.texto();
        Fecha nacimiento = l.fecha();
        Texto correo = l.texto();
        Texto rutaFoto = l.texto();
        Texto ciudad = l.texto();
        Texto pais = l.texto();
        Fecha inscripcion = l.fecha();

        if (idx > 0 && !(usuarios[idx - 1]->getNombreUsuario() < nombre)) {
            std::cerr << "Error: usuarios.txt debe estar ordenado por nombre "
                         "y sin repetidos (linea " << idx + 1 << ")\n";
            return false;
        }
        usuarios.agregar(new Usuario(nombre, clave, nacimiento, correo, rutaFoto,
                                     ciudad, pais, inscripcion, idx));
        ++idx;
    }
    if (!lecturaCompleta(f, "usuarios.txt")) return false;
    if (idx == 0) {
        std::cerr << "Error: usuarios.txt no tiene usuarios\n";
        return false;
    }
    amistades = new MatrizAmistad(idx);
    return true;
}

bool Plataforma::cargarAmistades()
{
    std::ifstream f;
    if (!abrirArchivo(f, "amistades.txt")) return false;
    int n = usuarios.getTamano();
    char linea[MAX_LINEA];
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;
        LectorLinea l(linea);
        int i = l.entero(), j = l.entero();
        if (i < 0 || j < 0 || i >= n || j >= n || i == j) {
            std::cerr << "Error: amistad invalida en amistades.txt\n";
            return false;
        }
        amistades->agregarAmistad(static_cast<short>(i), static_cast<short>(j));
    }
    if (!lecturaCompleta(f, "amistades.txt")) return false;
    return true;
}

bool Plataforma::cargarAlbumes()
{
    std::ifstream f;
    if (!abrirArchivo(f, "albumes.txt")) return false;
    int n = usuarios.getTamano();
    char linea[MAX_LINEA];
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;
        LectorLinea l(linea);
        int dueno = l.entero();
        Texto nombre = l.texto();
        char priv[8];
        l.campo(priv, 8);
        Texto ruta = l.texto();
        if (dueno < 0 || dueno >= n) {
            std::cerr << "Error: dueno invalido en albumes.txt\n";
            return false;
        }
        usuarios[dueno]->agregarAlbum(new Album(nombre, ruta, priv[0]));
    }
    if (!lecturaCompleta(f, "albumes.txt")) return false;
    return true;
}

bool Plataforma::cargarFotos()
{
    std::ifstream f;
    if (!abrirArchivo(f, "fotos.txt")) return false;
    int n = usuarios.getTamano();
    char linea[MAX_LINEA];
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;
        LectorLinea l(linea);
        int dueno = l.entero();
        int nAlbum = l.entero();
        Fecha captura = l.fecha();
        int tamano = l.entero();
        Texto ruta = l.texto();
        if (dueno < 0 || dueno >= n || nAlbum < 0 || tamano < 0
            || nAlbum >= usuarios[dueno]->getCantidadAlbumes()) {
            std::cerr << "Error: referencia invalida en fotos.txt\n";
            return false;
        }
        Foto* foto = new Foto(captura, static_cast<unsigned int>(tamano), ruta);
        while (!l.terminado()) {                 // likes separados por comas
            Medidor::contarIteracion();
            int quien = l.entero(',');
            if (quien < 0 || quien >= n) {
                std::cerr << "Error: like invalido en fotos.txt\n";
                delete foto;
                return false;
            }
            foto->alternarLike(static_cast<short>(quien));
        }
        usuarios[dueno]->getAlbum(nAlbum)->agregarFoto(foto);
    }
    if (!lecturaCompleta(f, "fotos.txt")) return false;
    return true;
}

bool Plataforma::cargarSolicitudes()
{
    std::ifstream f;
    if (!abrirArchivo(f, "solicitudes.txt")) return false;
    int n = usuarios.getTamano();
    char linea[MAX_LINEA];
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;
        LectorLinea l(linea);
        int receptor = l.entero(), emisor = l.entero();
        Fecha fecha = l.fecha();
        if (receptor < 0 || emisor < 0 || receptor >= n || emisor >= n) {
            std::cerr << "Error: indice invalido en solicitudes.txt\n";
            return false;
        }
        usuarios[receptor]->agregarSolicitud(new Solicitud(usuarios[emisor], fecha));
    }
    if (!lecturaCompleta(f, "solicitudes.txt")) return false;
    return true;
}

bool Plataforma::cargarMensajes()
{
    std::ifstream f;
    if (!abrirArchivo(f, "mensajes.txt")) return false;
    int n = usuarios.getTamano();
    char linea[MAX_LINEA];
    while (f.getline(linea, MAX_LINEA)) {
        Medidor::contarIteracion();
        LectorLinea::limpiar(linea);
        if (linea[0] == '\0') continue;
        LectorLinea l(linea);
        int emisor = l.entero(), receptor = l.entero();
        Fecha fecha = l.fecha();
        int leido = l.entero();
        Texto texto = l.resto();                 // el texto puede contener ';'
        if (emisor < 0 || receptor < 0 || emisor >= n || receptor >= n) {
            std::cerr << "Error: indice invalido en mensajes.txt\n";
            return false;
        }
        Mensaje* m = new Mensaje(usuarios[emisor], usuarios[receptor], texto, fecha);
        if (leido == 1) m->marcarLeido();
        // Condiciones que necesitan la busqueda binaria y el indice de no leidos:
        // por receptor, fechas no decrecientes y los no leidos al final.
        int nr = usuarios[receptor]->getCantidadRecibidos();
        if (nr > 0) {
            Mensaje* ultimo = usuarios[receptor]->getRecibido(nr - 1);
            if (fecha < ultimo->getFechaEnvio()) {
                std::cerr << "Error: mensajes.txt debe estar en orden cronologico "
                             "para cada receptor\n";
                delete m;
                return false;
            }
            if (!ultimo->estaLeido() && m->estaLeido()) {
                std::cerr << "Error: en mensajes.txt los mensajes no leidos deben "
                             "ir despues de los leidos (por receptor)\n";
                delete m;
                return false;
            }
        }
        usuarios[emisor]->enviarMensaje(m);
        usuarios[receptor]->recibirMensaje(m);   // llegan en orden cronologico
    }
    if (!lecturaCompleta(f, "mensajes.txt")) return false;
    for (int i = 0; i < n; ++i) {
        Medidor::contarIteracion();
        usuarios[i]->recalcularPrimerNoLeido();
    }
    return true;
}

bool Plataforma::cargarDatos()
{
    Medidor::reiniciar();
    bool ok = cargarUsuarios() && cargarAmistades() && cargarAlbumes()
              && cargarFotos() && cargarSolicitudes() && cargarMensajes();
    if (!ok) return false;
    std::cout << "Datos cargados: " << usuarios.getTamano() << " usuarios.\n";
    reportarRecursos(0);
    return true;
}

// =====================================================================
//  I. Guardado de datos (se escribe solo al cerrar sesion)
// =====================================================================
bool Plataforma::abrirSalida(std::ofstream& f, const char* nombre) const
{
    Texto ruta = rutaDatos + nombre;
    f.open(ruta.c_str());
    if (!f) {
        std::cerr << "Error: no se pudo escribir " << ruta << "\n";
        return false;
    }
    return true;
}

// Una linea "i;j" por amistad, con i < j.
bool Plataforma::guardarAmistades()
{
    std::ofstream f;
    if (!abrirSalida(f, "amistades.txt")) return false;
    int n = usuarios.getTamano();
    for (int i = 0; i < n; ++i) {
        Medidor::contarIteracion();
        Lista<short> amigos = amistades->amigosDe(static_cast<short>(i));
        for (int k = 0; k < amigos.getTamano(); ++k) {
            Medidor::contarIteracion();
            if (amigos[k] > i) f << i << ';' << amigos[k] << '\n';
        }
    }
    return f.good();
}

// receptor;emisor;fecha
bool Plataforma::guardarSolicitudes()
{
    std::ofstream f;
    if (!abrirSalida(f, "solicitudes.txt")) return false;
    for (int r = 0; r < usuarios.getTamano(); ++r) {
        Medidor::contarIteracion();
        Usuario* u = usuarios[r];
        for (int i = 0; i < u->getCantidadSolicitudes(); ++i) {
            Medidor::contarIteracion();
            Solicitud* s = u->getSolicitud(i);
            f << r << ';' << s->getEmisor()->getIndice() << ';'
              << s->getFechaEnvio() << '\n';
        }
    }
    return f.good();
}

// emisor;receptor;fecha;leido;texto
// Se escribe agrupado por RECEPTOR, siguiendo su lista de recibidos: asi cada
// receptor recupera sus mensajes en el mismo orden y con los no leidos al final.
bool Plataforma::guardarMensajes()
{
    std::ofstream f;
    if (!abrirSalida(f, "mensajes.txt")) return false;
    for (int r = 0; r < usuarios.getTamano(); ++r) {
        Medidor::contarIteracion();
        Usuario* u = usuarios[r];
        for (int i = 0; i < u->getCantidadRecibidos(); ++i) {
            Medidor::contarIteracion();
            Mensaje* m = u->getRecibido(i);
            f << m->getEmisor()->getIndice() << ';' << r << ';'
              << m->getFechaEnvio() << ';' << (m->estaLeido() ? 1 : 0) << ';'
              << m->getTexto() << '\n';
        }
    }
    return f.good();
}

// dueno;nAlbum;fecha;tamano;ruta;likes (indices separados por comas)
bool Plataforma::guardarFotos()
{
    std::ofstream f;
    if (!abrirSalida(f, "fotos.txt")) return false;
    for (int u = 0; u < usuarios.getTamano(); ++u) {
        Medidor::contarIteracion();
        Usuario* dueno = usuarios[u];
        for (int a = 0; a < dueno->getCantidadAlbumes(); ++a) {
            Medidor::contarIteracion();
            Album* album = dueno->getAlbum(a);
            for (int k = 0; k < album->getCantidadFotos(); ++k) {
                Medidor::contarIteracion();
                Foto* foto = album->getFoto(k);
                f << u << ';' << a << ';' << foto->getFechaCaptura() << ';'
                  << foto->getTamano() << ';' << foto->getRuta() << ';';
                for (unsigned int l = 0; l < foto->getLikes(); ++l) {
                    Medidor::contarIteracion();
                    if (l > 0) f << ',';
                    f << foto->getQuienDioLike(static_cast<int>(l));
                }
                f << '\n';
            }
        }
    }
    return f.good();
}

bool Plataforma::guardarDatos()
{
    Medidor::reiniciar();
    bool ok = guardarAmistades() && guardarSolicitudes()
              && guardarMensajes() && guardarFotos();
    if (ok) std::cout << "Cambios guardados.\n";
    else std::cerr << "Atencion: no se pudieron guardar todos los cambios.\n";
    reportarRecursos(sizeof(ok));
    return ok;
}

// =====================================================================
//  Utilidades
// =====================================================================
int Plataforma::buscarUsuario(const Texto& nombre) const
{
    int ini = 0, fin = usuarios.getTamano() - 1;
    while (ini <= fin) {
        Medidor::contarIteracion();
        int medio = ini + (fin - ini) / 2;
        const Texto& actual = usuarios[medio]->getNombreUsuario();
        if (actual == nombre) return medio;
        if (actual < nombre) ini = medio + 1;
        else fin = medio - 1;
    }
    return -1;
}

int Plataforma::aleatorio(int minimo, int maximo)
{
    Medidor::registrarUsoExterno();
    std::uniform_int_distribution<int> distribucion(minimo, maximo);
    return distribucion(generador);
}

unsigned long Plataforma::memoriaUsada() const
{
    unsigned long total = sizeof(*this) + usuarios.memoriaDinamica()
    + memoriaDinamica(rutaDatos);
    if (amistades != nullptr) total += amistades->memoriaUsada();
    for (int i = 0; i < usuarios.getTamano(); ++i) {
        Medidor::contarIteracion();
        total += usuarios[i]->memoriaUsada();
    }
    return total;
}

// Mide la memoria SIN contar las iteraciones de la propia medicion.
void Plataforma::reportarRecursos(unsigned long bytesLocales)
{
    Medidor::pausar();
    unsigned long bytes = memoriaUsada() + bytesLocales;
    Medidor::reanudar();
    Medidor::registrarBytes(bytes);
    Medidor::mostrarReporte();
}

// =====================================================================
//  II. Ingreso y mejores amigos
// =====================================================================
bool Plataforma::iniciarSesion(const Texto& nombre, const Texto& clave)
{
    Medidor::reiniciar();
    int pos = buscarUsuario(nombre);
    bool ok = pos >= 0 && usuarios[pos]->verificarContrasena(clave);
    if (ok) {
        sesionActiva = usuarios[pos];
        cantidadGrupo = 0;
    }
    unsigned long locales = sizeof(pos) + sizeof(ok)
                            + nombre.memoriaUsada() + clave.memoriaUsada();
    reportarRecursos(locales);
    return ok;
}

void Plataforma::cerrarSesion()
{
    sesionActiva = nullptr;
    cantidadGrupo = 0;
}

unsigned long Plataforma::generarGrupo()
{
    short yo = sesionActiva->getIndice();
    Lista<short> amigos = amistades->amigosDe(yo);
    int total = amigos.getTamano();
    if (total == 0) {
        cantidadGrupo = 0;
        return amigos.memoriaUsada() + sizeof(total);
    }
    int cuantos = total < 3 ? total : 3;

    // Peso segun amigos en comun contigo (enteros: 1 -> 2, 2.5 -> 5, 3.5 -> 7).
    short* pesos = new short[total];
    bool* tomado = new bool[total]();
    bool* excluido = new bool[total]();
    for (int i = 0; i < total; ++i) {
        Medidor::contarIteracion();
        short comunes = amistades->comunes(yo, amigos[i]);
        pesos[i] = comunes <= 10 ? 2 : (comunes <= 20 ? 5 : 7);
    }

    // El grupo nuevo no repite fotos del grupo anterior (si hay amigos de sobra).
    if (total - cantidadGrupo >= cuantos) {
        for (int i = 0; i < total; ++i)
            for (int g = 0; g < cantidadGrupo; ++g) {
                Medidor::contarIteracion();
                if (grupo[g] == amigos[i]) excluido[i] = true;
            }
    }

    short nuevo[3] = {-1, -1, -1};
    int suma = 0, r = 0, acumulado = 0, elegido = -1;
    for (int k = 0; k < cuantos; ++k) {
        // Ruleta sin reemplazo: dentro del grupo no se repite ningun amigo.
        suma = 0;
        for (int i = 0; i < total; ++i) {
            Medidor::contarIteracion();
            if (!tomado[i] && !excluido[i]) suma += pesos[i];
        }
        r = aleatorio(0, suma - 1);
        acumulado = 0;
        elegido = -1;
        for (int i = 0; i < total; ++i) {
            Medidor::contarIteracion();
            if (tomado[i] || excluido[i]) continue;
            acumulado += pesos[i];
            if (r < acumulado) { elegido = i; break; }
        }
        tomado[elegido] = true;
        nuevo[k] = amigos[elegido];
    }
    for (int k = 0; k < cuantos; ++k) {
        Medidor::contarIteracion();
        grupo[k] = nuevo[k];
    }
    cantidadGrupo = static_cast<short>(cuantos);

    unsigned long locales = amigos.memoriaUsada()
                            + total * (sizeof(short) + 2 * sizeof(bool)) + sizeof(nuevo)
                            + sizeof(yo) + sizeof(total) + sizeof(cuantos) + sizeof(suma) + sizeof(r)
                            + sizeof(acumulado) + sizeof(elegido);
    delete[] pesos;
    delete[] tomado;
    delete[] excluido;
    return locales;
}

void Plataforma::imprimirGrupo() const
{
    if (cantidadGrupo == 0) {
        std::cout << "(Aun no tienes amigos para mostrar)\n";
        return;
    }
    for (int g = 0; g < cantidadGrupo; ++g)
        std::cout << "Foto perfil de amigo " << g + 1 << ": "
                  << usuarios[grupo[g]]->getRutaFotoPerfil() << "\n";
}

void Plataforma::mostrarMejoresAmigos()
{
    Medidor::reiniciar();
    unsigned long locales = generarGrupo();
    imprimirGrupo();
    reportarRecursos(locales);
}

// =====================================================================
//  Menu y ciclo principal
// =====================================================================
void Plataforma::mostrarMenu() const
{
    std::cout << "\nBienvenido a UdeaBook " << sesionActiva->getNombreUsuario() << "\n\n"
              << "Menu de opciones:\n\n"
              << "1.- Administrar amistades\n"
              << "2.- Visitar perfil de un amigo\n"
              << "3.- Ver sugerencias de amistad\n"
              << "4.- Administrar mensajes\n\n"
              << ".- Ingrese 'N' o 'n' para alternar el grupo de fotos de perfil de amigos.\n"
              << ".- Ingrese 'S' o 's' para cerrar su sesion.\n\n"
              << "Su opcion: ";
}

void Plataforma::menuPrincipal()
{
    mostrarMejoresAmigos();
    char buffer[256];
    while (true) {
        mostrarMenu();
        if (!leerLinea(buffer, 256)) break;
        char op = opcion(buffer);
        if (op == 'S' || op == 's') break;
        if (op == 'N' || op == 'n') { mostrarMejoresAmigos(); continue; }
        if (op == '1') administrarAmistades();
        else if (op == '2') visitarPerfilAmigo();
        else if (op == '3') verSugerencias();
        else if (op == '4') administrarMensajes();
        else std::cout << "Opcion no valida.\n";
        std::cout << "\n";
        imprimirGrupo();
    }
    cerrarSesion();
    guardarDatos();
    std::cout << "Sesion cerrada.\n";
}

// =====================================================================
//  IV. Visitar el perfil de un amigo
// =====================================================================
void Plataforma::navegarAlbum(Album* album)
{
    int total = album->getCantidadFotos();
    if (total == 0) {
        std::cout << "El album no tiene fotos.\n";
        return;
    }
    short yo = sesionActiva->getIndice();
    int actual = 0;
    char buffer[256];
    while (true) {
        Foto* foto = album->getFoto(actual);
        std::cout << "\nFoto " << actual + 1 << " de " << total << ": ";
        foto->mostrarRuta();
        std::cout << "Likes: " << foto->getLikes()
                  << (foto->yaDioLike(yo) ? " (tu diste like)" : "") << "\n"
                  << "1.- Siguiente   2.- Previa   3.- Dar/quitar like   "
                  << "4.- Salir del album\nSu opcion: ";
        if (!leerLinea(buffer, 256)) return;
        char op = opcion(buffer);
        if (op == '1') actual = (actual + 1) % total;                // circular
        else if (op == '2') actual = (actual + total - 1) % total;   // circular
        else if (op == '3')
            std::cout << (foto->alternarLike(yo) ? "Diste like.\n"
                                                 : "Quitaste tu like.\n");
        else if (op == '4') return;
        else std::cout << "Opcion no valida.\n";
    }
}

void Plataforma::visitarPerfilAmigo()
{
    char buffer[256];
    std::cout << "Nombre de usuario del amigo: ";
    if (!leerLinea(buffer, 256)) return;

    Medidor::reiniciar();
    Texto nombre(buffer);
    int pos = buscarUsuario(nombre);
    short yo = sesionActiva->getIndice();
    if (pos < 0) {
        std::cout << "No existe el usuario " << nombre << ".\n";
        reportarRecursos(sizeof(pos) + nombre.memoriaUsada());
        return;
    }
    if (!amistades->sonAmigos(yo, static_cast<short>(pos))) {
        std::cout << nombre << " no es tu amigo: solo puedes visitar perfiles de amigos.\n";
        reportarRecursos(sizeof(pos) + nombre.memoriaUsada());
        return;
    }

    Usuario* amigo = usuarios[pos];
    int n = amigo->getCantidadAlbumes();
    short* visibles = new short[n];                 // albumes 'amigos' o 'publico'
    int cantidad = 0, elegido = 0;
    while (true) {
        cantidad = 0;
        std::cout << "\nAlbumes de " << nombre << ":\n";
        for (int i = 0; i < n; ++i) {
            Medidor::contarIteracion();
            Album* a = amigo->getAlbum(i);
            if (!a->esVisible(false, true)) continue;      // privados: no se ven
            visibles[cantidad++] = static_cast<short>(i);
            std::cout << cantidad << ".- " << a->getNombre() << " ("
                      << (a->getPrivacidad() == 'P' ? "publico" : "amigos") << ", "
                      << a->getCantidadFotos() << " fotos)\n";
        }
        if (cantidad == 0) {
            std::cout << "No tiene albumes visibles para ti.\n";
            break;
        }
        std::cout << "Numero del album a visitar (0 para volver): ";
        if (!leerLinea(buffer, 256)) break;
        if (!leerEnteroEstricto(buffer, elegido)) {
            std::cout << "Numero no valido.\n";
            continue;
        }
        if (elegido == 0) break;
        if (elegido < 1 || elegido > cantidad) {
            std::cout << "Numero no valido.\n";
            continue;
        }
        navegarAlbum(amigo->getAlbum(visibles[elegido - 1]));
    }
    unsigned long locales = sizeof(buffer) + sizeof(pos) + sizeof(yo) + sizeof(n)
                            + sizeof(cantidad) + sizeof(elegido) + sizeof(amigo) + sizeof(visibles)
                            + static_cast<unsigned long>(n) * sizeof(short) + nombre.memoriaUsada();
    reportarRecursos(locales);
    delete[] visibles;
}

// =====================================================================
//  VI. Mensajes
// =====================================================================
void Plataforma::enviarMensaje()
{
    char nombreBuf[256];
    char textoBuf[1024];
    std::cout << "Nombre de usuario del amigo: ";
    if (!leerLinea(nombreBuf, 256)) return;
    std::cout << "Mensaje (maximo " << Mensaje::MAX_CARACTERES << " caracteres): ";
    int estado = leerLineaLarga(textoBuf, 1024);
    if (estado < 0) return;

    Medidor::reiniciar();
    Texto nombre(nombreBuf);
    Texto contenido(estado == 1 ? "" : textoBuf);
    int pos = buscarUsuario(nombre);
    short yo = sesionActiva->getIndice();

    bool soloEspacios = true;
    for (int i = 0; i < contenido.size(); ++i) {
        Medidor::contarIteracion();
        if (contenido[i] != ' ' && contenido[i] != '\t') { soloEspacios = false; break; }
    }

    if (pos < 0) {
        std::cout << "No existe el usuario " << nombre << ".\n";
    } else if (!amistades->sonAmigos(yo, static_cast<short>(pos))) {
        std::cout << nombre << " no es tu amigo: solo puedes enviar mensajes a amigos.\n";
    } else if (estado == 1 || contenido.size() > Mensaje::MAX_CARACTERES) {
        std::cout << "El mensaje supera los " << Mensaje::MAX_CARACTERES << " caracteres.\n";
    } else if (soloEspacios) {
        std::cout << "No se puede enviar un mensaje vacio.\n";
    } else {
        Usuario* destino = usuarios[pos];
        // Los recibidos deben quedar en orden cronologico (busqueda binaria).
        Fecha fecha = Fecha::hoy();
        int nRecibidos = destino->getCantidadRecibidos();
        if (nRecibidos > 0
            && fecha < destino->getRecibido(nRecibidos - 1)->getFechaEnvio())
            fecha = destino->getRecibido(nRecibidos - 1)->getFechaEnvio();
        Mensaje* m = new Mensaje(sesionActiva, destino, contenido, fecha);
        sesionActiva->enviarMensaje(m);
        destino->recibirMensaje(m);
        std::cout << "Mensaje enviado a " << nombre << ".\n";
    }
    unsigned long locales = sizeof(nombreBuf) + sizeof(textoBuf) + sizeof(estado)
                            + sizeof(pos) + sizeof(yo) + sizeof(soloEspacios) + nombre.memoriaUsada()
                            + contenido.memoriaUsada();
    reportarRecursos(locales);
}

void Plataforma::leerMensajesNuevos()
{
    Medidor::reiniciar();
    int desde = sesionActiva->getPrimerNoLeido();
    int total = sesionActiva->getCantidadRecibidos();
    if (desde >= total) {
        std::cout << "No tienes mensajes nuevos.\n";
    } else {
        std::cout << "\nMensajes nuevos (" << total - desde << "):\n";
        for (int i = desde; i < total; ++i) {
            Medidor::contarIteracion();
            std::cout << *sesionActiva->getRecibido(i) << "\n";
        }
        sesionActiva->marcarNuevosComoLeidos();
    }
    reportarRecursos(sizeof(desde) + sizeof(total));
}

void Plataforma::buscarMensajesPorFecha()
{
    char buffer[256];
    Fecha desde, hasta;
    std::cout << "Fecha de inicio (dd/mm/aaaa): ";
    if (!leerLinea(buffer, 256)) return;
    Medidor::reiniciar();
    if (!leerFecha(buffer, desde)) {
        std::cout << "Fecha de inicio no valida.\n";
        reportarRecursos(sizeof(buffer) + 2 * sizeof(Fecha));
        return;
    }
    std::cout << "Fecha de fin (dd/mm/aaaa): ";
    if (!leerLinea(buffer, 256)) return;
    if (!leerFecha(buffer, hasta)) {
        std::cout << "Fecha de fin no valida.\n";
        reportarRecursos(sizeof(buffer) + 2 * sizeof(Fecha));
        return;
    }
    if (hasta < desde) {
        std::cout << "La fecha de fin no puede ser anterior a la de inicio.\n";
        reportarRecursos(sizeof(buffer) + 2 * sizeof(Fecha));
        return;
    }

    Lista<Mensaje*> resultado;
    sesionActiva->mensajesEnRango(desde, hasta, resultado);
    std::cout << "\nMensajes recibidos entre " << desde << " y " << hasta << ": "
              << resultado.getTamano() << "\n";
    for (int i = 0; i < resultado.getTamano(); ++i) {
        Medidor::contarIteracion();
        std::cout << *resultado[i] << "\n";
    }
    reportarRecursos(sizeof(buffer) + 2 * sizeof(Fecha) + resultado.memoriaUsada());
}

void Plataforma::administrarMensajes()
{
    char buffer[256];
    while (true) {
        std::cout << "\nAdministrar mensajes:\n"
                  << "1.- Enviar mensaje\n"
                  << "2.- Leer nuevos mensajes\n"
                  << "3.- Buscar mensajes por rango de fechas\n"
                  << "4.- Volver\n\nSu opcion: ";
        if (!leerLinea(buffer, 256)) return;
        if (opcion(buffer) == '1') enviarMensaje();
        else if (opcion(buffer) == '2') leerMensajesNuevos();
        else if (opcion(buffer) == '3') buscarMensajesPorFecha();
        else if (opcion(buffer) == '4') return;
        else std::cout << "Opcion no valida.\n";
    }
}

// =====================================================================
//  III. Administrar amistades
// =====================================================================
bool Plataforma::enviarSolicitudA(int destino)
{
    Usuario* otro = usuarios[destino];
    short yo = sesionActiva->getIndice();
    if (destino == yo) {
        std::cout << "No puedes enviarte una solicitud a ti mismo.\n";
        return false;
    }
    if (amistades->sonAmigos(yo, static_cast<short>(destino))) {
        std::cout << "Ya eres amigo de " << otro->getNombreUsuario() << ".\n";
        return false;
    }
    if (sesionActiva->tieneSolicitudDe(static_cast<short>(destino))) {
        std::cout << otro->getNombreUsuario() << " ya te envio una solicitud: "
                  << "aceptala en 'Ver y aceptar solicitudes recibidas'.\n";
        return false;
    }
    if (otro->tieneSolicitudDe(yo)) {
        std::cout << "Ya le enviaste una solicitud a " << otro->getNombreUsuario()
        << " y esta pendiente.\n";
        return false;
    }
    otro->agregarSolicitud(new Solicitud(sesionActiva, Fecha::hoy()));
    std::cout << "Solicitud enviada a " << otro->getNombreUsuario() << ".\n";
    return true;
}

void Plataforma::aceptarSolicitudes()
{
    Medidor::reiniciar();
    int cantidad = sesionActiva->getCantidadSolicitudes();
    if (cantidad == 0) {
        std::cout << "No tienes solicitudes pendientes.\n";
        reportarRecursos(sizeof(cantidad));
        return;
    }
    std::cout << "\nSolicitudes recibidas:\n";
    for (int i = 0; i < cantidad; ++i) {
        Medidor::contarIteracion();
        Solicitud* s = sesionActiva->getSolicitud(i);
        std::cout << i + 1 << ".- " << s->getEmisor()->getNombreUsuario()
                  << " (" << s->getFechaEnvio() << ")\n";
    }
    std::cout << "Numero de la solicitud a aceptar (0 para volver): ";
    char buffer[256];
    if (!leerLinea(buffer, 256)) return;
    int elegida = 0;
    bool valido = leerEnteroEstricto(buffer, elegida);
    if (valido && elegida >= 1 && elegida <= cantidad) {
        Usuario* emisor = sesionActiva->getSolicitud(elegida - 1)->getEmisor();
        amistades->agregarAmistad(sesionActiva->getIndice(), emisor->getIndice());
        std::cout << "Ahora eres amigo de " << emisor->getNombreUsuario() << ".\n";
        sesionActiva->quitarSolicitud(elegida - 1);
    } else if (!valido || elegida != 0) {
        std::cout << "Numero no valido.\n";
    }
    reportarRecursos(sizeof(cantidad) + sizeof(elegida) + sizeof(buffer)
                     + sizeof(valido) + sizeof(Usuario*));
}

void Plataforma::administrarAmistades()
{
    char buffer[256];
    while (true) {
        std::cout << "\nAdministrar amistades:\n"
                  << "1.- Enviar solicitud de amistad\n"
                  << "2.- Ver y aceptar solicitudes recibidas\n"
                  << "3.- Volver\n\nSu opcion: ";
        if (!leerLinea(buffer, 256)) return;
        if (opcion(buffer) == '1') {
            std::cout << "Nombre de usuario: ";
            if (!leerLinea(buffer, 256)) return;
            Medidor::reiniciar();
            Texto nombre(buffer);
            int pos = buscarUsuario(nombre);
            if (pos < 0) std::cout << "No existe el usuario " << nombre << ".\n";
            else enviarSolicitudA(pos);
            reportarRecursos(sizeof(pos) + nombre.memoriaUsada());
        } else if (opcion(buffer) == '2') {
            aceptarSolicitudes();
        } else if (opcion(buffer) == '3') {
            return;
        } else {
            std::cout << "Opcion no valida.\n";
        }
    }
}

// =====================================================================
//  V. Sugerencias de amistad
// =====================================================================
void Plataforma::verSugerencias()
{
    char buffer[256];
    std::cout << "Cuantas sugerencias desea ver (1 a 20)? ";
    if (!leerLinea(buffer, 256)) return;
    int k = 0;
    if (!leerEnteroEstricto(buffer, k) || k < 1 || k > 20) {
        std::cout << "Valor no valido: debe estar entre 1 y 20.\n";
        return;
    }

    Medidor::reiniciar();
    int n = usuarios.getTamano();
    short yo = sesionActiva->getIndice();

    // 1) Amigos en comun con cada candidato (no amigo, no uno mismo).
    short* comunes = new short[n]();
    for (int v = 0; v < n; ++v) {
        Medidor::contarIteracion();
        if (v == yo || amistades->sonAmigos(yo, static_cast<short>(v))) continue;
        comunes[v] = amistades->comunes(yo, static_cast<short>(v));
    }

    // 2) Los K mejores: K pasadas eligiendo el mayor (sin ordenar todo).
    short* elegidos = new short[k];
    short* valores = new short[k];
    int cuantos = 0;
    for (int r = 0; r < k; ++r) {
        int mejor = -1;
        for (int v = 0; v < n; ++v) {
            Medidor::contarIteracion();
            if (comunes[v] > 0 && (mejor < 0 || comunes[v] > comunes[mejor]))
                mejor = v;
        }
        if (mejor < 0) break;                       // no quedan candidatos
        elegidos[cuantos] = static_cast<short>(mejor);
        valores[cuantos] = comunes[mejor];
        comunes[mejor] = 0;
        ++cuantos;
    }

    if (cuantos == 0) {
        std::cout << "No hay sugerencias: tus amigos no tienen otros amigos.\n";
    } else {
        std::cout << "\nSugerencias de amistad:\n";
        for (int i = 0; i < cuantos; ++i) {
            Medidor::contarIteracion();
            std::cout << i + 1 << ".- " << usuarios[elegidos[i]]->getNombreUsuario()
                      << " (" << valores[i] << " amigos en comun)\n";
        }
    }

    unsigned long locales = static_cast<unsigned long>(n) * sizeof(short)
                            + 2UL * k * sizeof(short) + sizeof(buffer) + sizeof(k)
                            + sizeof(n) + sizeof(yo) + sizeof(cuantos) + sizeof(int);
    reportarRecursos(locales);

    if (cuantos > 0) {
        std::cout << "Numero de la sugerencia a la que desea enviar solicitud "
                     "(0 para volver): ";
        if (leerLinea(buffer, 256)) {
            int elegida = 0;
            bool valido = leerEnteroEstricto(buffer, elegida);
            if (valido && elegida >= 1 && elegida <= cuantos) {
                Medidor::reiniciar();
                enviarSolicitudA(elegidos[elegida - 1]);
                reportarRecursos(locales + sizeof(elegida) + sizeof(valido));
            } else if (!valido || elegida != 0) {
                std::cout << "Numero no valido.\n";
            }
        }
    }
    delete[] comunes;
    delete[] elegidos;
    delete[] valores;
}

void Plataforma::ejecutar()
{
    char buffer[256];
    while (true) {
        std::cout << "\n=== UdeABook ===\nUsuario (escriba 'salir' para terminar): ";
        if (!leerLinea(buffer, 256)) break;
        Texto nombre(buffer);
        if (nombre == Texto("salir")) break;
        std::cout << "Contrasena: ";
        if (!leerLinea(buffer, 256)) break;
        Texto clave(buffer);
        if (!iniciarSesion(nombre, clave)) {
            std::cout << "Usuario o contrasena incorrectos.\n";
            continue;
        }
        menuPrincipal();
    }
}