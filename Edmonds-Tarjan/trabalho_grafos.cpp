#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
using namespace std;

class Grafo_Direcionado{
    int vertice;                   // Número de vértices
    vector<vector<int>> aresta; // Matriz de adjacência

public:
    Grafo_Direcionado(int vertice);
    Grafo_Direcionado(vector<vector<int>> vec);
    void criaAdjacencia(int verticeU, int verticeV);

    vector<vector<int>> fecho();

    vector<vector<int>> reducaoTransitiva();

    void mostraGrafo_Direcionado();
};

Grafo_Direcionado::Grafo_Direcionado(vector<vector<int>> vec)
{
    this->vertice = vec.size();
    this->aresta = vector<vector<int>>(vec.size(), vector<int>(vec[0].size()));
    for (int i = 0; i < vec.size(); i++)
    {
        this->aresta.push_back(vec[i]);
    }
}

Grafo_Direcionado::Grafo_Direcionado(int vertice)
{
    this->vertice = vertice;
    aresta = vector<vector<int>>(vertice, vector<int>(vertice, false));
}

void inserirAresta(vector<vector<int>>& grafo, int verticeV, int verticeU) {
    // Verifica se os vértices v e w estão dentro dos limites da matriz
    int numVertices = grafo.size();
    if (verticeV < 1 || verticeV > numVertices || verticeU < 1 || verticeU > numVertices) {
        cout << "Vértices inválidos." << endl;
        return;
    }

    // Adiciona a aresta na matriz de adjacência
    grafo[verticeV - 1][verticeU - 1] = 1;
}

vector<vector<int>> remove_loop_grafo(const vector<vector<int>> &grafo){
    int tamanhoGrafo = grafo.size();
    vector<vector<int>> grafoSemLoop = grafo;

    int i = 0;
    while(i < tamanhoGrafo){
        if (grafo[i][i] == 1){
            grafoSemLoop[i][i] = 0; 
            //cout << grafoSemLoop[i][j] << endl;
        }
        i++;
    }
    return grafoSemLoop;
}
//usar a Busca em profundidade para remover ciclos.
void DFS(vector<vector<int>> &grafo, vector<vector<int>> &arestaRemovida, int verticeInicio, vector<bool> &visitado, stack<int> &caminho){
    visitado[verticeInicio] = true;
    caminho.push(verticeInicio);
    for (int i = 0; i < grafo.size(); i++){
        if (grafo[verticeInicio][i] == 1){
            if (!visitado[i]){
                DFS(grafo, arestaRemovida, i, visitado, caminho);
            }
            else{
                // Remove a aresta que fecha o ciclo
                grafo[verticeInicio][i] = 0;
                arestaRemovida[verticeInicio][i] = 1;
            }
        }
    }
    caminho.pop();
    visitado[verticeInicio] = false;
}

tuple<vector<vector<int>>, vector<vector<int>>> removeCiclo(const vector<vector<int>> &grafo){
    vector<vector<int>> aux = grafo;
    vector<vector<int>> arestasRemovidas(grafo.size(), vector<int>(grafo.size(), 0));
    int tamGrafo = grafo.size();
    vector<bool> visited(tamGrafo, false);
    stack<int> path;

    int i = 0;
    while(i < tamGrafo){
        if (!visited[i]){
            DFS(aux, arestasRemovidas, i, visited, path);
        }
        i++;
    }

    return make_tuple(aux, arestasRemovidas);
}


vector<vector<int>> juncaoDeGrafos(const vector<vector<int>> &grafo1, const vector<vector<int>> &grafo2){
    if (grafo1.size() != grafo2.size() || grafo1[0].size() != grafo2[0].size()){
         // As matrizes devem ter a mesma quantidade de linhas e colunas
         cout << "As matrizes de adjacência deve ter a mesma quantidade de linhas e colunas." << endl;
         return {};
    }
    int tamGrafo1 = grafo1.size();
    vector<vector<int>> grafoResultante(tamGrafo1, vector<int>(tamGrafo1, 0));
    for (int i = 0; i < tamGrafo1; i++){
        for (int j = 0; j < tamGrafo1; j++)        {
            grafoResultante[i][j] = grafo1[i][j] || grafo2[i][j];
        }
    }

    return grafoResultante;
}

// A ideia é acha um caminho de A até B, de B até C e ver se A chega em C passando por B
vector<vector<int>> achaFecho(const vector<vector<int>> &grafo){
    int tamGrafo = grafo.size();
    vector<vector<int>> copiaGrafo = grafo;
    for (int i = 0; i < tamGrafo; i++){
        for (int j = 0; j < tamGrafo; j++){
            for (int k = 0; k < tamGrafo; k++){
                copiaGrafo[j][k] = copiaGrafo[j][k] || (copiaGrafo[j][i] && copiaGrafo[i][k]);
            }
        }
    }
    return copiaGrafo;
}

vector<vector<int>> convercaoMatrizIncidencia(const vector<vector<int>> &grafo){
    int tamGrafo = grafo.size();
    vector<vector<int>> matrizIncidencia(tamGrafo, vector<int>(tamGrafo, 0));

    for (int i = 0; i < tamGrafo; i++){
        for (int j = 0; j < tamGrafo; j++){
            if (grafo[i][j] == 1){
                matrizIncidencia[i][j] = 1;
            }
        }
    }

    return matrizIncidencia;
}

vector<vector<int>> comparaMatriz(const vector<vector<int>> &grafo1, const vector<vector<int>> &grafo2){
    int linha = grafo1.size();
    int coluna = grafo1[0].size();
    vector<vector<int>> resposta(linha, vector<int>(coluna, 0));
    for (int i = 0; i < linha; i++){
        for (int j = 0; j < coluna; j++){
            if (grafo1[i][j] == 1 && grafo2[i][j] == 0){
                resposta[i][j] = 1;
            }
        }
    }
    return resposta;
}

void mostraGrafo(const vector<vector<int>> &grafo){
    int linha = grafo.size();
    int coluna = grafo[0].size();

    for (int l = 0; l < linha; ++l){
        for (int c = 0; c < coluna; ++c){
            cout << grafo[l][c] << "-";
        }
        cout << endl;
    }
}

void Grafo_Direcionado::criaAdjacencia(int verticeU, int verticeV){
    aresta[verticeU - 1][verticeV - 1] = true; 
}

vector<vector<int>> Grafo_Direcionado::fecho(){
    vector<vector<int>> reach = aresta;
    for (int i = 0; i < vertice; i++){
        for (int j = 0; j < vertice; j++){
            for (int k = 0; k < vertice; k++){
                reach[j][k] = reach[j][k] || (reach[j][i] && reach[i][k]);
            }
        }
    }
    return reach;
}

vector<vector<int>> Grafo_Direcionado::reducaoTransitiva(){
    vector<vector<int>> fechoTransitivo = fecho();
    vector<vector<int>> reducao = aresta;
    for (int i = 0; i < vertice; i++){
        for (int j = 0; j < vertice; j++){
            if (reducao[i][j]){
                for (int k = 0; k < vertice; k++){
                    if (k != i && k != j && fechoTransitivo[i][k] && fechoTransitivo[k][j] && !fechoTransitivo[j][k]){
                        reducao[i][j] = false;
                        break;
                    }
                }
            }
        }
    }
    return reducao;
}

static vector<vector<int>> matrizMultipla(vector<vector<int>> grafo1, vector<vector<int>> grafo2){

    int linhaGrafo1 = grafo1.size();
    int colunaGrafo1 = grafo1[0].size();
    int colunaGrafo2 = grafo2[0].size();

    vector<vector<int>> resposta(linhaGrafo1, vector<int>(colunaGrafo2, 0));

    for (int i = 0; i < linhaGrafo1; ++i){
        for (int j = 0; j < colunaGrafo2; ++j){
            for (int k = 0; k < colunaGrafo1; ++k){
                resposta[i][j] += grafo1[i][k] * grafo2[k][j];
            }
        }
    }

    return resposta;
}

void Grafo_Direcionado::mostraGrafo_Direcionado(){
    cout << "   ";
    int iterador = 0;
    while (iterador < vertice){
        cout << iterador + 1 << " ";
        iterador++;
    }
    cout << endl;

    for (int i = 0; i < vertice; i++){
        cout << i + 1 << "  ";
        for (int j = 0; j < vertice; j++){
            cout << aresta[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
   
    int numVertices = 5;
    vector<vector<int>> grafo(numVertices, vector<int>(numVertices,0));

    inserirAresta(grafo, 1, 2);
    inserirAresta(grafo, 1, 3);
    inserirAresta(grafo, 1, 4);
    inserirAresta(grafo, 1, 5);
    inserirAresta(grafo, 2, 4);
    inserirAresta(grafo, 3, 4);
    inserirAresta(grafo, 3, 5);
    inserirAresta(grafo, 4, 5);
    

    cout << "Grafo Original" << endl;
    mostraGrafo(grafo);

    vector<vector<int>> grafoSemLoop = remove_loop_grafo(grafo); 
    tuple<vector<vector<int>>, vector<vector<int>>> grafoSemCiclo = removeCiclo(grafoSemLoop); 
    vector<vector<int>> grafoDirecionado = get<0>(grafoSemCiclo);
    vector<vector<int>> arestasRemovidas = get<1>(grafoSemCiclo);
    vector<vector<int>> fechoTransitivo = achaFecho(grafoDirecionado);
    vector<vector<int>> matrizIncidencia = convercaoMatrizIncidencia(fechoTransitivo); 
    vector<vector<int>> matriz_Multipla = matrizMultipla(grafoDirecionado, matrizIncidencia); 
    vector<vector<int>> respostaComparacao = comparaMatriz(grafoDirecionado, matriz_Multipla); 

    vector<vector<int>> grafoFinal = juncaoDeGrafos(arestasRemovidas, respostaComparacao);
    cout << "Grafo Final"<< endl;
    mostraGrafo(grafoFinal);
    return 0;
}