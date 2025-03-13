#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 5;
vector<pair<int, int>> graph[N];
vector<pair<int, int>> spanning_tree;

int dfsNumber[N], lowlink[N], onStack[N];
stack<int> s;
int n, dfsNumberCounter = 0;

void strongconnect(int v) {
    dfsNumber[v] = dfsNumberCounter++;
    lowlink[v] = dfsNumber[v];
    onStack[v] = 1;
    s.push(v);

    for (auto u : graph[v]) {
        if (dfsNumber[u.first] == -1) {
            strongconnect(u.first);
            lowlink[v] = min(lowlink[v], lowlink[u.first]);
        } else if (onStack[u.first]) {
            lowlink[v] = min(lowlink[v], dfsNumber[u.first]);
        }
    }

    if (dfsNumber[v] == lowlink[v]) {
        int min_edge_weight = INT_MAX;
        while (true) {
            int u = s.top();
            s.pop();
            onStack[u] = 0;
            if (u == v) break;
            for (auto x : graph[u]) {
                if (x.first == v) {
                    min_edge_weight = min(min_edge_weight, x.second);
                }
            }
        }
        if (min_edge_weight != INT_MAX) {
            spanning_tree.push_back({v, min_edge_weight});
        }
    }
}

int main() {
    cin >> n;
    for (int i = 0; i < n - 1; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    memset(dfsNumber, -1, sizeof(dfsNumber));
    for (int i = 1; i <= n; i++) {
        if (dfsNumber[i] == -1) {
            strongconnect(i);
        }
    }

    for (auto edge : spanning_tree) {
        cout << edge.first << " " << edge.second << "\n";
    }

    return 0;
}