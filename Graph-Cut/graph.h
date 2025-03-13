#ifndef MAXFLOW_GRAPH_H
#define MAXFLOW_GRAPH_H

#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include <limits>

template <typename TCap>
class Graph {
public:
    // Estrutura de nó
    struct Node {
        TCap excess;  // Excesso de fluxo no nó
        int parent;   // Nó pai no caminho de aumento
        TCap cap;     // Capacidade da aresta
        TCap flow;    // Fluxo na aresta
    };

    // Construtor do grafo
    Graph(int n, int m) : numNodes(n), numEdges(m) {
        capacity.resize(n, std::vector<TCap>(n, 0));
        adjList.resize(n);
        nodes.resize(n);
    }

    // Adiciona aresta direcionada entre os nós
    void add_edge(int u, int v, TCap cap_u_v, TCap cap_v_u = 0) {
        capacity[u][v] += cap_u_v;
        capacity[v][u] += cap_v_u;
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    // Define as arestas entre um nó e a fonte ou poço
    void add_tweights(int node, TCap sourceWeight, TCap sinkWeight) {
        add_edge(source, node, sourceWeight);
        add_edge(node, sink, sinkWeight);
    }

    // Algoritmo de fluxo máximo usando o método de Boykov-Kolmogorov
    TCap maxflow() {
        TCap maxFlow = 0;

        // Enquanto houver caminho aumentante
        while (true) {
            std::vector<int> parent(numNodes, -1);
            std::vector<TCap> pathCapacity(numNodes, 0);
            std::queue<int> q;

            // Inicia a busca pela fonte
            q.push(source);
            pathCapacity[source] = std::numeric_limits<TCap>::max();

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                // Busca pelos vizinhos
                for (int v : adjList[u]) {
                    if (parent[v] == -1 && capacity[u][v] > 0) {
                        parent[v] = u;
                        pathCapacity[v] = std::min(pathCapacity[u], capacity[u][v]);
                        if (v == sink) {
                            break;
                        }
                        q.push(v);
                    }
                }
            }

            // Se não encontrou caminho aumentante, termina
            if (parent[sink] == -1) {
                break;
            }

            // Fluxo do caminho aumentante
            TCap flow = pathCapacity[sink];
            maxFlow += flow;

            // Atualiza as capacidades dos nós ao longo do caminho
            int v = sink;
            while (v != source) {
                int u = parent[v];
                capacity[u][v] -= flow;
                capacity[v][u] += flow;
                v = u;
            }
        }

        return maxFlow;
    }

    // Retorna a segmentação do nó
    int what_segment(int node) {
        return capacity[source][node] > 0 ? 1 : 0;
    }

    // Configura a fonte e o poço
    void set_source_sink(int s, int t) {
        source = s;
        sink = t;
    }

private:
    int numNodes;  // Número de nós no grafo
    int numEdges;  // Número de arestas no grafo
    int source, sink;  // Nó fonte e nó poço
    std::vector<std::vector<TCap>> capacity;  // Capacidades das arestas
    std::vector<std::vector<int>> adjList;  // Lista de adjacência
    std::vector<Node> nodes;  // Nós do grafo
};

#endif // MAXFLOW_GRAPH_H
