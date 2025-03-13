#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include "Grafo.h"
#include "Aresta.h"
#include "ImageUtils.h"
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
