#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_map>
#include "Aresta.h"
#include "Grafo.h"
#include "IFT.h"

using namespace cv;
using namespace std;

struct Pixel {
    int x, y;
    float custoCaminho;
    int predecessor;
    bool operator<(const Pixel& outro) const {
        return custoCaminho > outro.custoCaminho;
    }
};

float calcularPeso(const Vec3b& cor1, const Vec3b& cor2, int x1, int y1, int x2, int y2) {
    float diffB = cor1[0] - cor2[0];
    float diffG = cor1[1] - cor2[1];
    float diffR = cor1[2] - cor2[2];
    float distanciaCor = sqrt(diffB * diffB + diffG * diffG + diffR * diffR);
    float distanciaEspacial = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    return distanciaCor + distanciaEspacial;
}

Grafo criarGrafoIFT(const Mat& imagem) {
    int linhas = imagem.rows;
    int colunas = imagem.cols;
    Grafo grafo;
    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            int idx = i * colunas + j;
            if (i + 1 < linhas) {
                int vizinhoIdx = (i + 1) * colunas + j;
                float peso = calcularPeso(imagem.at<Vec3b>(i, j), imagem.at<Vec3b>(i + 1, j), i, j, i + 1, j);
                grafo.adicionarAresta(new Aresta(to_string(idx) + "-" + to_string(vizinhoIdx), peso, idx, vizinhoIdx));
            }
            if (j + 1 < colunas) {
                int vizinhoIdx = i * colunas + (j + 1);
                float peso = calcularPeso(imagem.at<Vec3b>(i, j), imagem.at<Vec3b>(i, j + 1), i, j, i, j + 1);
                grafo.adicionarAresta(new Aresta(to_string(idx) + "-" + to_string(vizinhoIdx), peso, idx, vizinhoIdx));
            }
            if (i - 1 >= 0) {
                int vizinhoIdx = (i - 1) * colunas + j;
                float peso = calcularPeso(imagem.at<Vec3b>(i, j), imagem.at<Vec3b>(i - 1, j), i, j, i - 1, j);
                grafo.adicionarAresta(new Aresta(to_string(idx) + "-" + to_string(vizinhoIdx), peso, idx, vizinhoIdx));
            }
            if (j - 1 >= 0) {
                int vizinhoIdx = i * colunas + (j - 1);
                float peso = calcularPeso(imagem.at<Vec3b>(i, j), imagem.at<Vec3b>(i, j - 1), i, j, i, j - 1);
                grafo.adicionarAresta(new Aresta(to_string(idx) + "-" + to_string(vizinhoIdx), peso, idx, vizinhoIdx));
            }
        }
    }
    return grafo;
}

vector<int> transformacaoFlorestaImagem(const Mat& imagem, Grafo& grafo, float limiar, const vector<Point>& sementes) {
    int linhas = imagem.rows;
    int colunas = imagem.cols;
    int V = linhas * colunas;
    vector<int> rotulos(V, -1);
    vector<float> custosCaminho(V, FLT_MAX);
    priority_queue<Pixel> pq;

    for (const auto& semente : sementes) {
        int idx = semente.y * colunas + semente.x;
        pq.push({ semente.y, semente.x, 0, idx });
        custosCaminho[idx] = 0;
        rotulos[idx] = idx;
    }

    while (!pq.empty()) {
        Pixel atual = pq.top();
        pq.pop();
        int u = atual.x * colunas + atual.y;
        for (Aresta* aresta : grafo.getAdjacencias(u)) {
            int v = (aresta->getVertice1() == u) ? aresta->getVertice2() : aresta->getVertice1();
            int vx = v / colunas;
            int vy = v % colunas;
            Vec3b corU = imagem.at<Vec3b>(atual.x, atual.y);
            Vec3b corV = imagem.at<Vec3b>(vx, vy);
            float peso = calcularPeso(corU, corV, atual.x, atual.y, vx, vy);
            if (peso > limiar) continue;
            float novoCusto = custosCaminho[u] + peso;
            if (novoCusto < custosCaminho[v]) {
                custosCaminho[v] = novoCusto;
                pq.push({ vx, vy, novoCusto, u });
                rotulos[v] = rotulos[u];
            }
        }
    }
    return rotulos;
}

Mat colorirSegmentosIFT(const Mat& imagem, const vector<int>& rotulos) {
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

void onMouse(int evento, int x, int y, int, void* userdata) {
    if (evento != EVENT_LBUTTONDOWN) return;
    vector<Point>* sementes = static_cast<vector<Point>*>(userdata);
    sementes->push_back(Point(x, y));
    cout << "Semente adicionada em: (" << x << ", " << y << ")" << endl;
}
