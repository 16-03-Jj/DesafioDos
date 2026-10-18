#ifndef FECHA_H
#define FECHA_H

#include <iostream>

class Fecha
{
private:
    short dia;
    short mes;
    short anio;

public:
    Fecha(short d = 1, short m = 1, short a = 2000);

    short getDia() const;
    short getMes() const;
    short getAnio() const;

    // true si dia, mes y anio forman una fecha real del calendario.
    bool esValida() const;

    // Comparaciones cronologicas (anio, luego mes, luego dia).
    bool operator<(const Fecha& otra) const;
    bool operator<=(const Fecha& otra) const;
    bool operator==(const Fecha& otra) const;

    // Fecha actual (hora de Colombia, UTC-5), calculada con <chrono>.
    static Fecha hoy();

    // Imprime dd/mm/aaaa.
    friend std::ostream& operator<<(std::ostream& os, const Fecha& f);
};

#endif // FECHA_H