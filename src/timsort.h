#ifndef TIMSORT_H
#define TIMSORT_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "Game.h"

namespace ts {
    constexpr unsigned MIN_PARTITION_SIZE = 32;
    // Standard insertion sort implementation. Takes a function pointer as the comparison operator
    void _insertionSort(std::vector<Game*>& data, bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
        // int actually isn't big enough; its size is only guarunteed to be up to 32,767.
        // data is around 200,000 elements in size

        // For each unsorted element (assumes first element is already sorted)
        for (long leftUnsortedIndex = 1; leftUnsortedIndex < static_cast<long>(data.size()); leftUnsortedIndex++) {
            // The key is the first unsorted element
            Game* key = data[leftUnsortedIndex];
            long rightSortedIndex = leftUnsortedIndex - 1;
            // Search for the appropriate key location
            // TODO perhaps use binary search here?
            while (rightSortedIndex >= 0 && lessThanComparator(key, data[rightSortedIndex])) {
                data[rightSortedIndex + 1] = data[rightSortedIndex];
                rightSortedIndex--;
            }
            // Move the key to the correct position
            data[rightSortedIndex + 1] = key;
        }
    }
}

#endif //TIMSORT_H
