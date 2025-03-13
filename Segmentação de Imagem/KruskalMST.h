#ifndef _KRUSKALMST_H_
#define _KRUSKALMST_H_


#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Aresta.h"
#include "Grafo.h"

using namespace cv;
using namespace std;
Mat carregarImagem(const string& caminhoImagem);
float calcularPeso(const Vec3b& cor1, const Vec3b& cor2);
vector<Aresta*> criarGrafoAGM(const Mat& imagem);
vector<Aresta*> kruskalMST(vector<Aresta*>& arestas, int V);
vector<int> segmentarImagemAGM(const vector<Aresta*>& mst, int V, float limiar, int tamanhoMinimo);
Mat colorirSegmentosAGM(const Mat& imagem, const vector<int>& rotulos);



#endif // _KRUSKALMST_H_