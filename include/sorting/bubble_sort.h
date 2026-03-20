#pragma once
#include <iterator>
#include <algorithm>

namespace algolib::sorting {

template<typename Iterator>
inline void bubbleSort(Iterator begin, Iterator end) {
    if (begin == end) return;

    bool swapped;
    for (Iterator i = begin; i != end; ++i) {
        swapped = false;
        for (Iterator j = begin; std::next(j) != end; ++j) {
            if (*std::next(j) < *j) {
                std::swap(*j, *std::next(j));
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

}