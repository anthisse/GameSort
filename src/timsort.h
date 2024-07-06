#ifndef TIMSORT_H
#define TIMSORT_H

#include <vector>
#include "Game.h"

namespace ts {
    constexpr unsigned MIN_PARTITION_SIZE = 32;


    // Binary insertion sort implementation. Takes a function pointer as the comparison operator
    void _binaryInsertionSort(std::vector<Game*>& games, bool (*lessThanComparator)(const Game* lhs, const Game* rhs));
}

#endif //TIMSORT_H
