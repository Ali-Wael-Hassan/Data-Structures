#pragma once
#include <cstdlib>
#include <ctime>
#include <stack>
#include <algorithm>


namespace algolib::sorting {

template<typename T>
inline void insertionSort(T* arr, size_t n) {
    if (n < 2) return;

    for (size_t i = 1; i < n; ++i) {
        T key = *(arr + i);
        T* j = arr + i; 

        while (j != arr && *(j - 1) > key) {
            *j = *(j - 1);
            --j;
        }
        *j = key;
    }
}

}