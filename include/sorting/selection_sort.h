#pragma once
#include <iterator>
#include <algorithm>

namespace algolib::sorting {

template<typename Iterator>
inline void selectionSort(Iterator begin, Iterator end) {
    if (begin == end) return;

    for (Iterator i = begin; i != end; ++i) {
        Iterator min_it = i;

        for (Iterator j = std::next(i); j != end; ++j) {
            if (*j < *min_it) {
                min_it = j;
            }
        }

        if (min_it != i) {
            std::iter_swap(i, min_it);
        }
    }
}

}