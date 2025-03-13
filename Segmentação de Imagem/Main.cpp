/**
 * @Main.cpp
 * @authors Jerson Vitor de Paula Gomes (https://github.com/JersonVitor) and Wallace Freitas Oliveira (https://github.com/Olivwallace)
 * @brief Ciclos em Grafos - Teoria dos Grafos e Computabilidade (PUC-Minas 1°/2024)
 * @date 13-03-2024
 */

// ---------- Inclusões
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "Aresta.h"
#include "Grafo.h"
#include "KruskalMST.h"
#include "IFT.h"

#define SUCESSO (0)
#define PATH "C:/Users/felli/Desktop/paris.jpg"

using namespace cv;
using namespace std;

const double WIDTH = 4.0;

int calculateKernelSize(double sigma) {
    sigma = std::max(sigma, 0.01);
    int length = static_cast<int>(std::ceil(sigma * WIDTH)) + 1;
    // O tamanho do kernel deve ser ímpar para garantir que haja um pixel central
    if (length % 2 == 0) {
        length += 1;
    }
    return length;
}

cv::Size generateKernelSize(double sigma) {
    int length = calculateKernelSize(sigma);
    return cv::Size(length, length);
}


void metodoKruskal() {

    string caminhoImagem = PATH; // Insira o caminho para a imagem
    Mat imagem = carregarImagem(caminhoImagem);
    Mat imagemFiltrada;
    GaussianBlur(imagem, imagemFiltrada, generateKernelSize(0.8), 0.8);

    vector<Aresta*> arestas = criarGrafoAGM(imagemFiltrada);
    int V = imagemFiltrada.rows * imagemFiltrada.cols;
    vector<Aresta*> mst = kruskalMST(arestas, V);

    float limiar = 15.0;
    int tamanhoMinimo = 4; // Tamanho mínimo de segmentos para não serem unidos

    vector<int> rotulos = segmentarImagemAGM(mst, V, limiar, tamanhoMinimo);
    Mat segmentadaAGM = colorirSegmentosAGM(imagemFiltrada, rotulos);

    imshow("Imagem Original", imagem);
    imshow("Imagem Segmentada", segmentadaAGM);
    waitKey(0);
}


void metodoIFT() {

    string imagePath = PATH; // Insira o caminho para a imagem
    Mat imagem = carregarImagem(imagePath);
    Mat imagemFiltrada;
    GaussianBlur(imagem, imagemFiltrada, generateKernelSize(0.8), 0.8);


    // Seleção de sementes manuais
    vector<Point> seeds;
    namedWindow("Original Image");
    setMouseCallback("Original Image", onMouse, &seeds);
    imshow("Original Image", imagem);
    waitKey(0);

    float threshold = 15.0; // Defina o valor do threshold

    Grafo graph = criarGrafoIFT(imagemFiltrada);
    vector<int> labels = transformacaoFlorestaImagem(imagemFiltrada, graph, threshold, seeds);
    Mat segmented = colorirSegmentosIFT(imagemFiltrada, labels);

    imshow("Segmented Image", segmented);
    waitKey(0);
}

int main() {

    metodoKruskal();
    metodoIFT();

    return SUCESSO;
}
