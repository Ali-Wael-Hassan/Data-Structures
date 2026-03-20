#pragma once
#include "../sorting/merge_sort.h"
#include "../sorting/quick_sort.h"
#include "../sorting/insertion_sort.h"

namespace algolib {

enum class SortAlgorithm {
    Merge,
    Quick,
    Insertion
};


template<typename Iterator>
inline void sort(Iterator begin, Iterator end, SortAlgorithm algo = SortAlgorithm::Merge) {
    size_t n = std::distance(begin, end);
    if (n < 2) return;

    switch (algo) {
        case SortAlgorithm::Merge:
            sorting::mergeSortIterative(begin, n);
            break;
        case SortAlgorithm::Quick:
            sorting::quickSortIterative(begin, n);
            break;
        case SortAlgorithm::Insertion:
            sorting::insertionSort(begin, end);
            break;
    }
}

} // namespace algolib