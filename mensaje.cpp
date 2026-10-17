#include "Mensaje.h"
#include "Usuario.h"

Mensaje::Mensaje(Usuario* quienEnvia, Usuario* quienRecibe,
                 const Texto& contenido, const Fecha& fecha)
    : emisor(quienEnvia), receptor(quienRecibe), texto(contenido),
    fechaEnvio(fecha), leido(false)
{
}

Mensaje::Mensaje(const Mensaje& otro)
    : emisor(otro.emisor), receptor(otro.receptor), texto(otro.texto),
    fechaEnvio(otro.fechaEnvio), leido(otro.leido)
{
}

Usuario* Mensaje::getEmisor() const        { return emisor; }
Usuario* Mensaje::getReceptor() const      { return receptor; }
const Texto& Mensaje::getTexto() const     { return texto; }
const Fecha& Mensaje::getFechaEnvio() const { return fechaEnvio; }
bool Mensaje::estaLeido() const            { return leido; }
void Mensaje::marcarLeido()                { leido = true; }

bool Mensaje::operator<(const Mensaje& otro) const
{
    return fechaEnvio < otro.fechaEnvio;
}

std::ostream& operator<<(std::ostream& os, const Mensaje& m)
{
    return os << "[" << m.fechaEnvio << "] "
              << m.emisor->getNombreUsuario() << ": " << m.texto;
}

unsigned long Mensaje::memoriaUsada() const
{
    return sizeof(*this) + memoriaDinamica(texto);
}
