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
    static unsigned long usosExternos;   // invocaciones a componentes externos
    static bool contando;                // false mientras se mide la memoria

public:
    // Pone los contadores en cero (llamar al iniciar cada funcionalidad).
    static void reiniciar();

    // Cuenta una iteracion (llamar dentro de CADA ciclo del programa).
    static void contarIteracion();

    // Suma n iteraciones de una vez.
    static void sumarIteraciones(unsigned long n);

    // Registra una invocacion a un componente externo (p. ej. <random>).
    static void registrarUsoExterno();

    // Suspende / reanuda el conteo de iteraciones. Se usa mientras se calcula
    // la memoria, para que medir no altere la cuenta de la funcionalidad.
    static void pausar();
    static void reanudar();

    // Fija el total de memoria (en bytes) consumida en este instante.
    static void registrarBytes(unsigned long bytes);

    static unsigned long getIteraciones();
    static unsigned long getBytes();

    // Muestra en pantalla las metricas pedidas por el enunciado.
    static void mostrarReporte();
};

#endif // MEDIDOR_H
