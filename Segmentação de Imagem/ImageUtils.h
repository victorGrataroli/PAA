#pragma once
#ifndef _ImageUtils_H_
#define _ImageUtils_H_


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
#endif // _ImageUtils_H_