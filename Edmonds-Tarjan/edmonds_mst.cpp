#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 5;
vector<pair<int, int>> graph[N];

int match[N], dist[N];
bool visited[N];

bool dfs(int u) {
    if (visited[u]) return false;
    visited[u] = true;

    for (auto v : graph[u]) {
        if (match[v.first] == -1 || dfs(match[v.first])) {
            match[v.first] = u;
            return true;
        }
    }

    return false;
}

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    memset(match, -1, sizeof(match));
    int result = 0;
    for (int u = 1; u <= n; u++) {
        fill(dist, dist + n + 1, -1);
        dist[0] = 0;
        queue<int> q;
        q.push(0);
        bool updated = true;
        while (!q.empty() && updated) {
            updated = false;
            int d = q.front();
            q.pop();
            for (auto v : graph[u]) {
                if (dist[v.first] == -1 && match[u] != v.first) {
                    dist[v.first] = d + 1;
                    q.push(v.first);
                    updated = true;
                }
            }
        }
        fill(visited, visited + n + 1, false);
        if (dfs(u)) {
            result += dist[match[u]];
        }
    }

    cout << result << "\n";

    return 0;
}