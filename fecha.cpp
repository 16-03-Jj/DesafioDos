#include "Fecha.h"
#include <iomanip>

Fecha::Fecha(short d, short m, short a) : dia(d), mes(m), anio(a) {}

short Fecha::getDia() const  { return dia; }
short Fecha::getMes() const  { return mes; }
short Fecha::getAnio() const { return anio; }

bool Fecha::operator<(const Fecha& otra) const
{
    if (anio != otra.anio) return anio < otra.anio;
    if (mes != otra.mes)   return mes < otra.mes;
    return dia < otra.dia;
}

bool Fecha::operator<=(const Fecha& otra) const
{
    return !(otra < *this);
}

bool Fecha::operator==(const Fecha& otra) const
{
    return dia == otra.dia && mes == otra.mes && anio == otra.anio;
}

std::ostream& operator<<(std::ostream& os, const Fecha& f)
{
    char previo = os.fill('0');
    os << std::setw(2) << f.dia << '/' << std::setw(2) << f.mes << '/'
       << std::setw(4) << f.anio;
    os.fill(previo);
    return os;
}
