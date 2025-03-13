#ifndef _ARESTA_H_
#define _ARESTA_H_
#pragma once

#include <iostream>
#include <string>

using namespace std;

class Aresta {
    string id;
    float peso;
    int vertice1, vertice2;

public:
    Aresta(string id, float peso, int vertice1, int vertice2)
        : id(id), peso(peso), vertice1(vertice1), vertice2(vertice2) {}

    string getID() { return id; }
    float getPeso() { return peso; }
    int getVertice1() { return vertice1; }
    int getVertice2() { return vertice2; }
};

#endif // _ARESTA_H_
