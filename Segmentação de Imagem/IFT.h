#pragma once
#ifndef _ITF_H_
#define _ITF_H_


#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Aresta.h"
#include "Grafo.h"

using namespace cv;
using namespace std;

float calcularPeso(const Vec3b& color1, const Vec3b& color2, int x1, int y1, int x2, int y2);
Grafo criarGrafoIFT(const Mat& image);
vector<int> transformacaoFlorestaImagem(const Mat& image, Grafo& graph, float threshold, const vector<Point>& seeds);
Mat colorirSegmentosIFT(const Mat& image, const vector<int>& labels);
void onMouse(int event, int x, int y, int, void* userdata);
#endif // _ITF_H_