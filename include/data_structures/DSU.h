#pragma once
#include <algorithm>
#include "vector.h"

namespace algolib {

class DSU {
private:
    vector<int> parent, rank, size;
    int components;

public:
    inline explicit DSU(int n)
        : parent(n), rank(n, 0), size(n, 1), components(n)
    {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    inline int find(int x) {
        return parent[x] == x ? x : (parent[x] = find(parent[x]));
    }

    inline bool same(int a, int b) {
        return find(a) == find(b);
    }

    inline int getSize(int x) {
        return size[find(x)];
    }

    inline int count() {
        return components;
    }

    inline int merge(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return -1;

        --components;
        if (rank[a] > rank[b]) std::swap(a, b);
        parent[a] = b;
        size[b] += size[a];
        if (rank[a] == rank[b]) ++rank[b];
        return b;
    }
};

} // namespace algolib