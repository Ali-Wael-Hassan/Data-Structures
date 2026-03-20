#pragma once
#include "../sorting/merge_sort.h"
#include "../sorting/quick_sort.h"

namespace algolib {

enum class SortAlgorithm {
    Merge,
    Quick
};


template<typename T>
inline constexpr void sort(T* begin, T* end, SortAlgorithm algo = SortAlgorithm::Merge) {
    switch (algo) {
        case SortAlgorithm::Merge:
            sorting::mergeSortIterative(begin, end - begin);
            break;
        case SortAlgorithm::Quick:
            sorting::quickSort(begin, end - begin);
            break;
    }
}

} // namespace algolib