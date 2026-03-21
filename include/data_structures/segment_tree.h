#pragma once

#include <iterator>

namespace algolib {

template<typename T>
struct plus {
    T operator()(const T& a, const T& b) const { return a + b; }
};

template<class T, class MergeFunc = plus<T>>
class segment_tree {
private:
    T* tree;
    int n;
    T identity;
    MergeFunc merge;

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

    void update_internal(int node, int start, int end, int idx, const T& val) {
        if (start == end) {
            tree[node] = val;
            return;
        }
        int mid = (start + end) >> 1;
        if (idx <= mid) 
            update_internal(2 * node + 1, start, mid, idx, val);
        else 
            update_internal(2 * node + 2, mid + 1, end, idx, val);
        
        tree[node] = merge(tree[2 * node + 1], tree[2 * node + 2]);
    }

    T query_internal(int node, int start, int end, int L, int R) const {
        if (R < start || end < L) return identity;
        
        if (L <= start && end <= R) return tree[node];

        int mid = (start + end) >> 1;
        return merge(query_internal(2 * node + 1, start, mid, L, R),
                     query_internal(2 * node + 2, mid + 1, end, L, R));
    }

public:

    template<class Iterator>
    segment_tree(Iterator begin, Iterator end, T identityElement, MergeFunc func) 
        : n(static_cast<int>(std::distance(begin, end))), 
          identity(identityElement), 
          merge(func) {
        
        if (n > 0) {
            tree = new T[4 * n](); 
            build_internal(begin, 0, 0, n - 1);
        } else {
            tree = nullptr;
        }
    }

    segment_tree(int size, T identityElement, MergeFunc func)
        : n(size),
          identity(identityElement),
          merge(func) {
        
        if (n > 0) {
            tree = new T[4 * n];
            for (int i = 0; i < 4 * n; ++i) tree[i] = identity;
        } else {
            tree = nullptr;
        }
    }

    ~segment_tree() {
        if(tree) delete[] tree;
    }

    segment_tree(const segment_tree&) = delete;
    segment_tree& operator=(const segment_tree&) = delete;

    void update(int idx, const T& val) {
        if (idx >= 0 && idx < n) {
            update_internal(0, 0, n - 1, idx, val);
        }
    }

    T query(int L, int R) const {
        if (L < 0 || R >= n || L > R) return identity;
        return query_internal(0, 0, n - 1, L, R);
    }
    
    int size() const { return n; }
};

} // namespace algolib