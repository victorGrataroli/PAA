#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

// Estrutura para representar uma aresta
struct Aresta {
    int u, v;         // Vértices conectados pela aresta
    float peso;       // Peso da aresta

    bool operator<(const Aresta& outra) const {
        return peso < outra.peso;
    }
};

// Estrutura para representar um conjunto disjunto
class ConjuntoDisjunto {
public:
    std::vector<int> pai;
    std::vector<int> rank;
    std::vector<int> tamanho;

    ConjuntoDisjunto(int n) : pai(n), rank(n, 0), tamanho(n, 1) {
        for (int i = 0; i < n; ++i) pai[i] = i;
    }

    int encontrar(int x) {
        if (pai[x] != x) {
            pai[x] = encontrar(pai[x]); // Compressão de caminho
        }
        return pai[x];
    }

    void unir(int x, int y) {
        int raizX = encontrar(x);
        int raizY = encontrar(y);

        if (raizX != raizY) {
            if (rank[raizX] < rank[raizY]) {
                pai[raizX] = raizY;
                tamanho[raizY] += tamanho[raizX];
            } else if (rank[raizX] > rank[raizY]) {
                pai[raizY] = raizX;
                tamanho[raizX] += tamanho[raizY];
            } else {
                pai[raizY] = raizX;
                tamanho[raizX] += tamanho[raizY];
                ++rank[raizX];
            }
        }
    }

    int obterTamanho(int x) {
        return tamanho[encontrar(x)];
    }
};

// Função para calcular o limiar adaptativo
float limiar(int tamanho, float k) {
    return k / tamanho;
}

// Função principal para segmentação de grafos
std::vector<int> segmentarGrafo(int numVertices, std::vector<Aresta>& arestas, float k) {
    // Ordenar as arestas por peso
    std::sort(arestas.begin(), arestas.end());

    // Inicializar conjuntos disjuntos
    ConjuntoDisjunto cd(numVertices);

    // Vetor para armazenar a diferença interna máxima de cada componente
    std::vector<float> diferencaInterna(numVertices, 0.0f);

    // Processar as arestas em ordem
    for (const auto& aresta : arestas) {
        int u = aresta.u;
        int v = aresta.v;
        float peso = aresta.peso;

        int compU = cd.encontrar(u);
        int compV = cd.encontrar(v);

        if (compU != compV) {
            float limiarMinimo = std::min(diferencaInterna[compU] + limiar(cd.obterTamanho(compU), k),
                                          diferencaInterna[compV] + limiar(cd.obterTamanho(compV), k));

            if (peso <= limiarMinimo) {
                cd.unir(compU, compV);
                int novaComp = cd.encontrar(compU);
                diferencaInterna[novaComp] = std::max({diferencaInterna[compU], diferencaInterna[compV], peso});
            }
        }
    }

    // Gerar o vetor de componentes finais
    std::vector<int> componentes(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        componentes[i] = cd.encontrar(i);
    }

    return componentes;
}

// Função para converter imagem em grafo
std::vector<Aresta> imagemParaGrafo(const cv::Mat& imagem, int& numVertices) {
    numVertices = imagem.rows * imagem.cols;
    std::vector<Aresta> arestas;

    auto indice = [&imagem](int x, int y) { return x * imagem.cols + y; };

    for (int i = 0; i < imagem.rows; ++i) {
        for (int j = 0; j < imagem.cols; ++j) {
            if (j + 1 < imagem.cols) {
                float peso = std::abs(imagem.at<uchar>(i, j) - imagem.at<uchar>(i, j + 1));
                arestas.push_back({indice(i, j), indice(i, j + 1), peso});
            }
            if (i + 1 < imagem.rows) {
                float peso = std::abs(imagem.at<uchar>(i, j) - imagem.at<uchar>(i + 1, j));
                arestas.push_back({indice(i, j), indice(i + 1, j), peso});
            }
        }
    }

    return arestas;
}

int main() {
    // Caminho para a pasta de imagens
    std::string pastaImagens = "imagens";

    // Iterar por todos os arquivos na pasta
    for (const auto& entrada : fs::directory_iterator(pastaImagens)) {
        std::string caminhoImagem = entrada.path().string();

        // Carregar a imagem em escala de cinza
        cv::Mat imagem = cv::imread(caminhoImagem, cv::IMREAD_GRAYSCALE);
        if (imagem.empty()) {
            std::cerr << "Erro ao carregar a imagem: " << caminhoImagem << std::endl;
            continue;
        }

        int numVertices;
        std::vector<Aresta> arestas = imagemParaGrafo(imagem, numVertices);

        float k = 300.0f; // Parâmetro de escala

        std::vector<int> componentes = segmentarGrafo(numVertices, arestas, k);

        // Criar uma imagem de saída para visualização
        cv::Mat saida(imagem.size(), CV_32S);
        for (int i = 0; i < imagem.rows; ++i) {
            for (int j = 0; j < imagem.cols; ++j) {
                saida.at<int>(i, j) = componentes[i * imagem.cols + j];
            }
        }

        // Normalizar os componentes para visualização
        double minVal, maxVal;
        cv::minMaxLoc(saida, &minVal, &maxVal);
        cv::Mat exibicao;
        saida.convertTo(exibicao, CV_8U, 255.0 / (maxVal - minVal));

        // Salvar e mostrar o resultado
        std::string nomeSaida = "saida_" + entrada.path().filename().string();
        cv::imwrite(nomeSaida, exibicao);
        cv::imshow("Segmentação - " + entrada.path().filename().string(), exibicao);
        cv::waitKey(0);
    }

    return 0;
}
