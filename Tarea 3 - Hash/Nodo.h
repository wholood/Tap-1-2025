// Nodo.h
#ifndef NODO_H
#define NODO_H

struct Nodo {
    int clave;
    int valor;
    bool estaActivo;

    Nodo(int k, int v) : clave(k), valor(v), estaActivo(true) {}
};

#endif