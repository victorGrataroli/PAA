#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "graph.h"  // Biblioteca de max-flow, como a de Boykov-Kolmogorov

// Função para calcular os pesos das arestas baseados nas diferenças de intensidade entre os pixels
float calcularPesoAresta(const cv::Vec3b& pixel1, const cv::Vec3b& pixel2, float sigma = 10.0) {
    float diferenca = cv::norm(pixel1 - pixel2);
    return exp(-diferenca * diferenca / (2 * sigma * sigma));
}

void exibirInstrucoes() {
    std::cout << "\nINSTRUÇÕES:\n";
    std::cout << "1. Use o mouse para marcar regiões de objeto e fundo.\n";
    std::cout << "2. Regiões marcadas em vermelho (tecla 'o') indicam objeto.\n";
    std::cout << "3. Regiões marcadas em azul (tecla 'b') indicam fundo.\n";
    std::cout << "4. Pressione 's' para salvar a segmentação ou 'q' para sair.\n\n";
}

void marcarSeeds(cv::Mat& imagem, cv::Mat& mascaraSeeds, int x, int y, bool ehObjeto) {
    cv::circle(mascaraSeeds, cv::Point(x, y), 5, ehObjeto ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 0, 255), -1);
    cv::circle(imagem, cv::Point(x, y), 5, ehObjeto ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0), -1);
    cv::imshow("Imagem", imagem);
}

void aoClicarMouse(int evento, int x, int y, int flags, void* dadosUsuario) {
    static bool ehObjeto = true;
    std::pair<cv::Mat*, cv::Mat*>* dados = static_cast<std::pair<cv::Mat*, cv::Mat*>*>(dadosUsuario);
    cv::Mat& imagem = *dados->first;
    cv::Mat& mascaraSeeds = *dados->second;

    if (evento == cv::EVENT_LBUTTONDOWN) {
        marcarSeeds(imagem, mascaraSeeds, x, y, ehObjeto);
    } else if (evento == cv::EVENT_RBUTTONDOWN) {
        ehObjeto = !ehObjeto;
        std::cout << "Marcando como " << (ehObjeto ? "OBJETO" : "FUNDO") << ".\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <caminhos_para_imagens>\n";
        return -1;
    }

    // Lê imagens da entrada
    std::vector<cv::Mat> imagens;
    for (int i = 1; i < argc; ++i) {
        cv::Mat imagem = cv::imread(argv[i], cv::IMREAD_COLOR);
        if (imagem.empty()) {
            std::cerr << "Erro ao carregar imagem: " << argv[i] << "\n";
            return -1;
        }
        imagens.push_back(imagem);
    }

    for (auto& imagem : imagens) {
        int linhas = imagem.rows;
        int colunas = imagem.cols;

        cv::Mat mascaraSeeds = cv::Mat::zeros(linhas, colunas, CV_8UC3);
        cv::namedWindow("Imagem", cv::WINDOW_AUTOSIZE);
        exibirInstrucoes();

        std::pair<cv::Mat*, cv::Mat*> dadosUsuario(&imagem, &mascaraSeeds);
        cv::setMouseCallback("Imagem", aoClicarMouse, &dadosUsuario);

        while (true) {
            cv::imshow("Imagem", imagem);
            int tecla = cv::waitKey(0);

            if (tecla == 'q') {
                break;
            } else if (tecla == 's') {
                cv::imwrite("segmentacao.png", mascaraSeeds);
                std::cout << "Segmentação salva como 'segmentacao.png'.\n";
            }
        }

        // Criar um grafo
        typedef Graph<float, float, float> TipoGrafo;
        TipoGrafo grafo(linhas * colunas, 4 * linhas * colunas);

        // Adicionar nós ao grafo
        grafo.add_node(linhas * colunas);

        // Adicionar arestas n-links (entre pixels vizinhos)
        for (int y = 0; y < linhas; ++y) {
            for (int x = 0; x < colunas; ++x) {
                int atual = y * colunas + x;
                if (x + 1 < colunas) {
                    int direita = y * colunas + (x + 1);
                    float peso = calcularPesoAresta(imagem.at<cv::Vec3b>(y, x), imagem.at<cv::Vec3b>(y, x + 1));
                    grafo.add_edge(atual, direita, peso, peso);
                }
                if (y + 1 < linhas) {
                    int abaixo = (y + 1) * colunas + x;
                    float peso = calcularPesoAresta(imagem.at<cv::Vec3b>(y, x), imagem.at<cv::Vec3b>(y + 1, x));
                    grafo.add_edge(atual, abaixo, peso, peso);
                }
            }
        }

        // Adicionar arestas t-links (seeds de objeto e fundo)
        for (int y = 0; y < linhas; ++y) {
            for (int x = 0; x < colunas; ++x) {
                int atual = y * colunas + x;

                cv::Vec3b valorSeed = mascaraSeeds.at<cv::Vec3b>(y, x);
                if (valorSeed == cv::Vec3b(255, 0, 0)) {
                    grafo.add_tweights(atual, 1e9, 0); // Seed de objeto
                } else if (valorSeed == cv::Vec3b(0, 0, 255)) {
                    grafo.add_tweights(atual, 0, 1e9); // Seed de fundo
                }
            }
        }

        // Executar max-flow e obter corte mínimo
        float fluxo = grafo.maxflow();
        std::cout << "Fluxo máximo: " << fluxo << "\n";

        // Criar máscara de segmentação
        cv::Mat segmentacao(linhas, colunas, CV_8UC1);
        for (int y = 0; y < linhas; ++y) {
            for (int x = 0; x < colunas; ++x) {
                int atual = y * colunas + x;
                segmentacao.at<uchar>(y, x) = (grafo.what_segment(atual) == TipoGrafo::SOURCE) ? 255 : 0;
            }
        }

        // Exibir e salvar resultado
        cv::imshow("Segmentação", segmentacao);
        cv::waitKey(0);
    }

    return 0;
}

