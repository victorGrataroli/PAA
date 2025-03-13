#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>

// Função recursiva para encontrar a menor combinação de moedas usando força bruta
void encontraMelhorCombinacao(const std::vector<int>& moedas, int valor, std::vector<int>& melhorCombinacao, std::vector<int>& atualCombinacao, int inicio) {
    if (valor == 0) {
        // Se a combinação atual é menor que a melhor encontrada até agora, substituímos
        if (melhorCombinacao.empty() || atualCombinacao.size() < melhorCombinacao.size()) {
            melhorCombinacao = atualCombinacao;
        }
        return;
    }
    
    // Tentar usar cada moeda começando da moeda atual
    for (int i = inicio; i < moedas.size(); i++) {
        if (moedas[i] <= valor) {
            atualCombinacao.push_back(moedas[i]);
            encontraMelhorCombinacao(moedas, valor - moedas[i], melhorCombinacao, atualCombinacao, i); // Permite reutilizar a moeda
            atualCombinacao.pop_back(); // Backtrack
        }
    }
}

// Função para o algoritmo de força bruta refinado
std::vector<int> trocoForcaBruta(const std::vector<int>& moedas, int valor) {
    std::vector<int> melhorCombinacao;
    std::vector<int> atualCombinacao;
    encontraMelhorCombinacao(moedas, valor, melhorCombinacao, atualCombinacao, 0);
    return melhorCombinacao;
}

// Função para o algoritmo guloso
std::vector<int> trocoGuloso(const std::vector<int>& moedas, int valor) {
    std::vector<int> resultado;
    std::vector<int> moedasOrdenadas = moedas;
    std::sort(moedasOrdenadas.rbegin(), moedasOrdenadas.rend()); // Ordena em ordem decrescente

    for (int moeda : moedasOrdenadas) {
        while (valor >= moeda) {
            valor -= moeda;
            resultado.push_back(moeda);
        }
    }
    return resultado;
}

// Função para exibir o resultado
void exibirResultado(const std::string& metodo, const std::vector<int>& resultado) {
    std::cout << metodo << ": ";
    for (int moeda : resultado) {
        std::cout << moeda << " ";
    }
    std::cout << std::endl;
}

int main() {
    // Testando com o sistema de moedas canônico e não canônico
    std::vector<int> moedasCanonicas = {1, 5, 10, 25};  // Sistema de moedas canônico
    std::vector<int> moedasNaoCanonicas = {1, 3, 4};     // Sistema de moedas não canônico
    int valorCanonico = 63;
    int valorNaoCanonico = 6;

    // Algoritmo guloso
    std::vector<int> gulosoCanonico = trocoGuloso(moedasCanonicas, valorCanonico);
    std::vector<int> gulosoNaoCanonico = trocoGuloso(moedasNaoCanonicas, valorNaoCanonico);

    // Algoritmo de força bruta refinado
    std::vector<int> forcaBrutaCanonico = trocoForcaBruta(moedasCanonicas, valorCanonico);
    std::vector<int> forcaBrutaNaoCanonico = trocoForcaBruta(moedasNaoCanonicas, valorNaoCanonico);

    // Exibindo resultados
    exibirResultado("Guloso Canonico", gulosoCanonico);
    exibirResultado("Forca Bruta Canonico", forcaBrutaCanonico);
    exibirResultado("Guloso Nao-Canonico", gulosoNaoCanonico);
    exibirResultado("Forca Bruta Nao-Canonico", forcaBrutaNaoCanonico);

    return 0;
}
