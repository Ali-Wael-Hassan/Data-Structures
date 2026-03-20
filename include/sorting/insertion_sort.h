#pragma once
#include <cstdlib>
#include <ctime>
#include <stack>
#include <algorithm>


namespace algolib::sorting {

template<typename Iterator>
inline void insertionSort(Iterator begin, Iterator end) {
    if (begin == end) return;

    for (Iterator i = begin + 1; i != end; ++i) {
        auto key = *i;
        Iterator j = i;

        while (j != begin && *(j - 1) > key) {
            *j = *(j - 1);
            --j;
        }
        
        *j = key;
    }
}

}