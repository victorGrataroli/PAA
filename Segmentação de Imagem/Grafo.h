#ifndef _GRAFO_H_
#define _GRAFO_H_
#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Aresta.h"

using namespace std;

class Grafo {
    vector<Aresta*> arestas;
    unordered_map<int, vector<Aresta*>> adjacencias;

public:
    ~Grafo() {
        for (auto aresta : arestas) {
            delete aresta;
        }
    }

    void adicionarAresta(Aresta* aresta) {
        arestas.push_back(aresta);
        adjacencias[aresta->getVertice1()].push_back(aresta);
        adjacencias[aresta->getVertice2()].push_back(aresta);
    }

    vector<Aresta*> getArestas() {
        return arestas;
    }

    vector<Aresta*> getAdjacencias(int vertice) {
        return adjacencias[vertice];
    }
};

#endif // _GRAFO_H_
