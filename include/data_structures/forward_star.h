#pragma once
#include <cstring>

namespace algolib {

template<typename T, T N, T M>
class forward_star {
private:
    T head[N];
    T to[M], wt[M], nxt[M];
    T ne;

public:
    forward_star() {
        memset(head, -1, sizeof(head));
        ne = 0;
    }

    void clear(T n) {
        ne = 0;
        memset(head, -1, n * sizeof(head[0]));
    }

    void addEdge(T u, T v, T w = 0) {
        to[ne] = v;
        wt[ne] = w;
        nxt[ne] = head[u];
        head[u] = ne++;
    }

    void addBiEdge(T u, T v, T w = 0) {
        addEdge(u, v, w);
        addEdge(v, u, w);
    }

    T get_head(T u) const { return head[u]; }
    T get_to(T e) const { return to[e]; }
    T get_wt(T) const { return wt[e]; }
    T get_nxt(T e) const { return nxt[e]; }
};

} // namespace algolib

#define neigh(g, u, e, v) \
    for(ll e = g.get_head(u), v; ~e && (v = g.get_to(e), 1); e = g.get_nxt(e))