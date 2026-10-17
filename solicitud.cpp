#include "Solicitud.h"

Solicitud::Solicitud(Usuario* quienEnvia, const Fecha& fecha)
    : emisor(quienEnvia), fechaEnvio(fecha)
{
}

Solicitud::Solicitud(const Solicitud& otra)
    : emisor(otra.emisor), fechaEnvio(otra.fechaEnvio)
{
}

Usuario* Solicitud::getEmisor() const         { return emisor; }
const Fecha& Solicitud::getFechaEnvio() const { return fechaEnvio; }

unsigned long Solicitud::memoriaUsada() const
{
    return sizeof(*this);
}