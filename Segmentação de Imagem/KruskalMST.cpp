#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include "Grafo.h"
#include "Aresta.h"

using namespace cv;
using namespace std;

Mat carregarImagem(const string& caminhoImagem) {
    Mat imagem = imread(caminhoImagem, IMREAD_COLOR);
    if (imagem.empty()) {
        cerr << "Erro ao carregar a imagem!" << endl;
        exit(EXIT_FAILURE);
    }
    return imagem;
}

float calcularPeso(const Vec3b& cor1, const Vec3b& cor2) {
    float diffB = cor1[0] - cor2[0];
    float diffG = cor1[1] - cor2[1];
    float diffR = cor1[2] - cor2[2];
    float distanciaCor = sqrt(diffB * diffB + diffG * diffG + diffR * diffR);

    float intensidade1 = 0.299 * cor1[2] + 0.587 * cor1[1] + 0.114 * cor1[0];
    float intensidade2 = 0.299 * cor2[2] + 0.587 * cor2[1] + 0.114 * cor2[0];
    float diferencaIntensidade = abs(intensidade1 - intensidade2);

    float peso = distanciaCor + diferencaIntensidade;
    return peso;
}

vector<Aresta*> criarGrafoAGM(const Mat& imagem) {
    vector<Aresta*> arestas;
    int linhas = imagem.rows;
    int colunas = imagem.cols;
    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            Vec3b cor1 = imagem.at<Vec3b>(i, j);
            if (i + 1 < linhas) {
                Vec3b cor2 = imagem.at<Vec3b>(i + 1, j);
                float peso = calcularPeso(cor1, cor2);
                arestas.push_back(new Aresta(to_string(i * colunas + j) + "-" + to_string((i + 1) * colunas + j), peso, i * colunas + j, (i + 1) * colunas + j));
            }
            if (j + 1 < colunas) {
                Vec3b cor2 = imagem.at<Vec3b>(i, j + 1);
                float peso = calcularPeso(cor1, cor2);
                arestas.push_back(new Aresta(to_string(i * colunas + j) + "-" + to_string(i * colunas + (j + 1)), peso, i * colunas + j, i * colunas + (j + 1)));
            }
        }
    }
    return arestas;
}

struct ConjuntosDisjuntos {
    vector<int> pai, rank;
    ConjuntosDisjuntos(int n) : pai(n), rank(n, 0) {
        for (int i = 0; i < n; ++i) pai[i] = i;
    }
    int encontrar(int u) {
        return pai[u] == u ? u : (pai[u] = encontrar(pai[u]));
    }
    void unir(int u, int v) {
        u = encontrar(u), v = encontrar(v);
        if (rank[u] > rank[v]) pai[v] = u;
        else pai[u] = v;
        if (rank[u] == rank[v]) rank[v]++;
    }
};

vector<Aresta*> kruskalMST(vector<Aresta*>& arestas, int V) {
    sort(arestas.begin(), arestas.end(), [](Aresta* a, Aresta* b) { return a->getPeso() < b->getPeso(); });
    ConjuntosDisjuntos ds(V);
    vector<Aresta*> mst;
    for (Aresta* aresta : arestas) {
        int u = aresta->getVertice1();
        int v = aresta->getVertice2();
        if (ds.encontrar(u) != ds.encontrar(v)) {
            mst.push_back(aresta);
            ds.unir(u, v);
        }
    }
    return mst;
}

vector<int> segmentarImagemAGM(const vector<Aresta*>& mst, int V, float limiar, int tamanhoMinimo) {
    vector<int> rotulos(V);
    iota(rotulos.begin(), rotulos.end(), 0); // Inicializa os rótulos com 0, 1, 2, ...
    ConjuntosDisjuntos ds(V);
    for (Aresta* aresta : mst) {
        if (aresta->getPeso() > limiar) continue;
        int u = aresta->getVertice1();
        int v = aresta->getVertice2();
        if (ds.encontrar(u) != ds.encontrar(v)) {
            ds.unir(u, v);
        }
    }
    for (int i = 0; i < V; ++i) {
        rotulos[i] = ds.encontrar(i);
    }

    // Segunda fase: unir pequenos segmentos adjacentes
    map<int, int> tamanhoSegmento;
    for (int r : rotulos) {
        tamanhoSegmento[r]++;
    }

    for (Aresta* aresta : mst) {
        int u = aresta->getVertice1();
        int v = aresta->getVertice2();
        int raizU = ds.encontrar(u);
        int raizV = ds.encontrar(v);
        if (raizU != raizV && (tamanhoSegmento[raizU] < tamanhoMinimo || tamanhoSegmento[raizV] < tamanhoMinimo)) {
            ds.unir(raizU, raizV);
        }
    }

    for (int i = 0; i < V; ++i) {
        rotulos[i] = ds.encontrar(i);
    }

    return rotulos;
}

Mat colorirSegmentosAGM(const Mat& imagem, const vector<int>& rotulos) {
    int linhas = imagem.rows;
    int colunas = imagem.cols;
    Mat segmentada(linhas, colunas, CV_8UC3);
    unordered_map<int, Vec3b> cores;
    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            int rotulo = rotulos[i * colunas + j];
            if (cores.find(rotulo) == cores.end()) {
                Vec3b novaCor(rand() % 256, rand() % 256, rand() % 256);
                cores[rotulo] = novaCor;
            }
            segmentada.at<Vec3b>(i, j) = cores[rotulo];
        }
    }
    return segmentada;
}


