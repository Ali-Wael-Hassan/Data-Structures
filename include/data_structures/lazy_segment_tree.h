#pragma once

#include <iterator>
#include <algorithm>

namespace algolib {

template<typename T>
struct plus {
    T operator()(const T& a, const T& b) const { return a + b; }
};

template<typename T>
struct default_apply {
    void operator()(T& node, const T& update, int len) const {
        node += update * len;
    }
};

template<class T, class MergeFunc = plus<T>, class ApplyFunc = default_apply<T>>
class lazy_segment_tree {
private:
    T* tree;
    T* lazy;
    int n;
    T identity;
    T lazy_identity;
    MergeFunc merge;
    ApplyFunc apply;

    void push(int node, int start, int end) {
        if (lazy[node] == lazy_identity) return;

        if (start != end) {
            int mid = (start + end) >> 1;

            apply(tree[2 * node + 1], lazy[node], mid - start + 1);
            lazy[2 * node + 1] += lazy[node];

            apply(tree[2 * node + 2], lazy[node], end - mid);
            lazy[2 * node + 2] += lazy[node];
        }

        lazy[node] = lazy_identity;
    }

    template<class Iterator>
    void build_internal(Iterator begin, int node, int start, int end) {
        if (start == end) {
            tree[node] = *(begin + start);
            return;
        }
        int mid = (start + end) >> 1;
        build_internal(begin, 2 * node + 1, start, mid);
        build_internal(begin, 2 * node + 2, mid + 1, end);
        tree[node] = merge(tree[2 * node + 1], tree[2 * node + 2]);
    }

    void update_range_internal(int node, int start, int end, int L, int R, const T& val) {
        push(node, start, end);

        if (start > end || start > R || end < L) return;

        if (start >= L && end <= R) {
            apply(tree[node], val, end - start + 1);
            lazy[node] += val;
            return;
        }

        int mid = (start + end) >> 1;
        update_range_internal(2 * node + 1, start, mid, L, R, val);
        update_range_internal(2 * node + 2, mid + 1, end, L, R, val);
        tree[node] = merge(tree[2 * node + 1], tree[2 * node + 2]);
    }

    T query_internal(int node, int start, int end, int L, int R) {
        push(node, start, end);

        if (R < start || end < L) return identity;
        if (L <= start && end <= R) return tree[node];

        int mid = (start + end) >> 1;
        return merge(query_internal(2 * node + 1, start, mid, L, R),
                     query_internal(2 * node + 2, mid + 1, end, L, R));
    }

public:
    // Constructor for Iterators
    template<class Iterator>
    lazy_segment_tree(Iterator begin, Iterator end, T identityElement, T lazyId = T(), 
                      MergeFunc mFunc = MergeFunc(), ApplyFunc aFunc = ApplyFunc()) 
        : n(static_cast<int>(std::distance(begin, end))), 
          identity(identityElement), 
          lazy_identity(lazyId),
          merge(mFunc),
          apply(aFunc) {
        
        if (n > 0) {
            tree = new T[4 * n](); 
            lazy = new T[4 * n]();
            for(int i=0; i<4*n; ++i) lazy[i] = lazy_identity;
            build_internal(begin, 0, 0, n - 1);
        } else {
            tree = lazy = nullptr;
        }
    }

    lazy_segment_tree(int size, T identityElement, T lazyId = T(), 
                      MergeFunc mFunc = MergeFunc(), ApplyFunc aFunc = ApplyFunc())
        : n(size),
          identity(identityElement),
          lazy_identity(lazyId),
          merge(mFunc),
          apply(aFunc) {
        
        if (n > 0) {
            tree = new T[4 * n];
            lazy = new T[4 * n];
            for (int i = 0; i < 4 * n; ++i) {
                tree[i] = identity;
                lazy[i] = lazy_identity;
            }
        } else {
            tree = lazy = nullptr;
        }
    }

    ~lazy_segment_tree() {
        if(tree) delete[] tree;
        if(lazy) delete[] lazy;
    }

    void update(int L, int R, const T& val) {
        if (L <= R && L >= 0 && R < n) {
            update_range_internal(0, 0, n - 1, L, R, val);
        }
    }

    T query(int L, int R) {
        if (L < 0 || R >= n || L > R) return identity;
        return query_internal(0, 0, n - 1, L, R);
    }
};

} // namespace algolib