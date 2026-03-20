#pragma once
#include <algorithm>
#include <cstddef>
#include <iterator>

namespace algolib::sorting {

template<typename T>
inline constexpr void merge(T* in, T* out, T* left, T* mid, T* right) {
    T* p = out + (left - in);
    T* l = left;
    T* r = mid;

    while (l != mid && r != right) *p++ = (*l < *r) ? *l++ : *r++;
    while (l != mid) *p++ = *l++;
    while (r != right) *p++ = *r++;
}

template<typename Iterator>
inline void mergeSortIterative(Iterator begin, size_t n) {
    if (n < 2) return;

    typedef typename std::iterator_traits<Iterator>::value_type T;
    T* arr = &(*begin);
    T* buffer = new T[n];
    T* in = arr;
    T* out = buffer;

    for (size_t width = 1; width < n; width *= 2) {
        for (size_t i = 0; i < n; i += 2 * width) {
            size_t left = i;
            size_t mid = std::min(i + width, n);
            size_t right = std::min(i + 2 * width, n);
            merge(in, out, in + left, in + mid, in + right);
        }
        std::swap(in, out);
    }

    if (in != arr) std::copy(in, in + n, arr);

    delete[] buffer;
}

}