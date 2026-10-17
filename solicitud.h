#ifndef SOLICITUD_H
#define SOLICITUD_H

#include "Fecha.h"

class Usuario;   // declaracion adelantada

// Solicitud de amistad PENDIENTE. Vive en la lista del usuario RECEPTOR;
// por eso solo necesita recordar quien la envio y cuando.
class Solicitud
{
private:
    Usuario* emisor;
    Fecha fechaEnvio;

public:
    Solicitud(Usuario* quienEnvia, const Fecha& fecha);
    Solicitud(const Solicitud& otra);

    Usuario* getEmisor() const;
    const Fecha& getFechaEnvio() const;

    unsigned long memoriaUsada() const;
};

#endif // SOLICITUD_H
