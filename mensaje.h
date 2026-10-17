#ifndef MENSAJE_H
#define MENSAJE_H

#include <iostream>
#include "Fecha.h"
#include "Texto.h"

class Usuario;   // declaracion adelantada: Mensaje solo guarda punteros

class Mensaje
{
private:
    Usuario* emisor;
    Usuario* receptor;
    Texto texto;
    Fecha fechaEnvio;
    bool leido;

public:
    static const int MAX_CARACTERES = 512;

    Mensaje(Usuario* quienEnvia, Usuario* quienRecibe,
            const Texto& contenido, const Fecha& fecha);
    Mensaje(const Mensaje& otro);

    Usuario* getEmisor() const;
    Usuario* getReceptor() const;
    const Texto& getTexto() const;
    const Fecha& getFechaEnvio() const;
    bool estaLeido() const;
    void marcarLeido();

    // Orden cronologico (por fecha de envio).
    bool operator<(const Mensaje& otro) const;

    // Imprime: [dd/mm/aaaa] emisor: texto
    friend std::ostream& operator<<(std::ostream& os, const Mensaje& m);

    unsigned long memoriaUsada() const;
};

#endif // MENSAJE_H
