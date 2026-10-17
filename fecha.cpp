#include "Fecha.h"
#include <chrono>
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

Fecha Fecha::hoy()
{
    using namespace std::chrono;
    long long horas = duration_cast<hours>(system_clock::now().time_since_epoch()).count();
    long long dias = (horas - 5) / 24;            // UTC-5 (Colombia, sin horario de verano)
    if ((horas - 5) % 24 < 0) --dias;

    // Dias desde 1970-01-01 -> fecha del calendario gregoriano.
    long long z = dias + 719468;
    long long era = (z >= 0 ? z : z - 146096) / 146097;
    long long doe = z - era * 146097;
    long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long long anio = yoe + era * 400;
    long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long long mp = (5 * doy + 2) / 153;
    long long dia = doy - (153 * mp + 2) / 5 + 1;
    long long mes = mp < 10 ? mp + 3 : mp - 9;
    if (mes <= 2) ++anio;
    return Fecha(static_cast<short>(dia), static_cast<short>(mes), static_cast<short>(anio));
}
