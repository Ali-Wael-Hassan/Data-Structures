#include <cstdlib>
#include <ctime>
#include <stack>
#include <algorithm>

template<typename T>
inline void quickSortIterative(T* arr, size_t n) {
    if (n <= 1) return;

    std::srand(std::time(nullptr));

    struct Range { T* left; T* right; };
    std::stack<Range> stk;
    stk.push({ arr, arr + n });

    while (!stk.empty()) {
        Range range = stk.top();
        stk.pop();

        T* left = range.left;
        T* right = range.right;

        if (right - left <= 1) continue;

        size_t pivot_offset = std::rand() % (right - left);
        T* pivot = left + pivot_offset;
        T pivot_val = *pivot;
        std::swap(*pivot, *(right - 1));

        T* store = left;
        for (T* it = left; it < right - 1; ++it) {
            if (*it < pivot_val) std::swap(*it, *store++);
        }
        std::swap(*(pivot), *store);

        if (store - left > right - (store + 1)) {
            if (left < store) stk.push({ left, store });
            if (store + 1 < right) stk.push({ store + 1, right });
        } else {
            if (store + 1 < right) stk.push({ store + 1, right });
            if (left < store) stk.push({ left, store });
        }
    }
} // namespace sorting