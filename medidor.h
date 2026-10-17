#ifndef MEDIDOR_H
#define MEDIDOR_H

// Mide el consumo de recursos de cada funcionalidad.
// Los contadores son de clase (compartidos), de modo que cualquier clase
// (incluida la plantilla Lista) puede reportar iteraciones sin recibir
// referencias adicionales.
class Medidor
{
private:
    static unsigned long iteraciones;
    static unsigned long bytesEnUso;

public:
    // Pone ambos contadores en cero (llamar al iniciar cada funcionalidad).
    static void reiniciar();

    // Cuenta una iteracion (llamar dentro de CADA ciclo del programa).
    static void contarIteracion();

    // Suma n iteraciones de una vez (util al invocar componentes externos).
    static void sumarIteraciones(unsigned long n);

    // Fija el total de memoria (en bytes) consumida en este instante.
    static void registrarBytes(unsigned long bytes);

    static unsigned long getIteraciones();
    static unsigned long getBytes();

    // Muestra en pantalla las dos metricas pedidas por el enunciado.
    static void mostrarReporte();
};

#endif // MEDIDOR_H
