#pragma once
#include <algorithm>
#include "vector.h"

namespace algolib {

template<typename T = int>
class DSU {
private:
    vector<T> parent, rank, size;
    T components;

public:
    inline explicit DSU(T n)
        : parent(n), rank(n, 0), size(n, 1), components(n)
    {
        for (T i = 0; i < n; ++i) parent[i] = i;
    }

    inline T find(T x) {
        return parent[x] == x ? x : (parent[x] = find(parent[x]));
    }

    inline bool same(T a, T b) {
        return find(a) == find(b);
    }

    inline T getSize(T x) {
        return size[find(x)];
    }

    inline T count() {
        return components;
    }

    inline T merge(T a, T b) {
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