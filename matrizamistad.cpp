#include "MatrizAmistad.h"

// Conteo de bits en uno sin ciclos (tecnica de sumas por bloques).
short MatrizAmistad::contarBits(unsigned long long x)
{
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return static_cast<short>((x * 0x0101010101010101ULL) >> 56);
}

MatrizAmistad::MatrizAmistad(short numUsuarios)
    : n(numUsuarios), palabrasPorFila((numUsuarios + 63) / 64)
{
    int total = n * palabrasPorFila;
    bits = new unsigned long long[total];
    for (int k = 0; k < total; ++k) {
        Medidor::contarIteracion();
        bits[k] = 0;
    }
}

MatrizAmistad::MatrizAmistad(const MatrizAmistad& otra)
    : n(otra.n), palabrasPorFila(otra.palabrasPorFila)
{
    int total = n * palabrasPorFila;
    bits = new unsigned long long[total];
    for (int k = 0; k < total; ++k) {
        Medidor::contarIteracion();
        bits[k] = otra.bits[k];
    }
}

MatrizAmistad::~MatrizAmistad()
{
    delete[] bits;
}

MatrizAmistad& MatrizAmistad::operator=(const MatrizAmistad& otra)
{
    if (this != &otra) {
        int total = otra.n * otra.palabrasPorFila;
        unsigned long long* nuevo = new unsigned long long[total];
        for (int k = 0; k < total; ++k) {
            Medidor::contarIteracion();
            nuevo[k] = otra.bits[k];
        }
        delete[] bits;
        bits = nuevo;
        n = otra.n;
        palabrasPorFila = otra.palabrasPorFila;
    }
    return *this;
}

void MatrizAmistad::agregarAmistad(short i, short j)
{
    bits[i * palabrasPorFila + j / 64] |= (1ULL << (j % 64));
    bits[j * palabrasPorFila + i / 64] |= (1ULL << (i % 64));
}

bool MatrizAmistad::sonAmigos(short i, short j) const
{
    return (bits[i * palabrasPorFila + j / 64] >> (j % 64)) & 1ULL;
}

short MatrizAmistad::comunes(short i, short j) const
{
    short total = 0;
    const unsigned long long* filaI = bits + i * palabrasPorFila;
    const unsigned long long* filaJ = bits + j * palabrasPorFila;
    for (short w = 0; w < palabrasPorFila; ++w) {
        Medidor::contarIteracion();
        total += contarBits(filaI[w] & filaJ[w]);
    }
    return total;
}

short MatrizAmistad::contarAmigos(short i) const
{
    short total = 0;
    const unsigned long long* fila = bits + i * palabrasPorFila;
    for (short w = 0; w < palabrasPorFila; ++w) {
        Medidor::contarIteracion();
        total += contarBits(fila[w]);
    }
    return total;
}

Lista<short> MatrizAmistad::amigosDe(short i) const
{
    Lista<short> amigos;
    const unsigned long long* fila = bits + i * palabrasPorFila;
    for (short w = 0; w < palabrasPorFila; ++w) {
        Medidor::contarIteracion();
        if (fila[w] == 0) continue;          // salta palabras vacias
        for (short b = 0; b < 64; ++b) {
            Medidor::contarIteracion();
            if ((fila[w] >> b) & 1ULL)
                amigos.agregar(static_cast<short>(w * 64 + b));
        }
    }
    return amigos;
}

unsigned long MatrizAmistad::memoriaUsada() const
{
    return sizeof(*this)
    + static_cast<unsigned long>(n) * palabrasPorFila * sizeof(unsigned long long);
}
