#include <algorithm>
#include <cstdint>
#include "timsort.h"

namespace ts {
    void _binaryInsertionSort(std::vector<Game*>& games, bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
        // int actually isn't big enough; its size is only guarunteed to be up to 32,767.
        // data is around 200,000 elements in size
        // For each unsorted element (assumes first element is already sorted)
        for (int32_t leftUnsortedIndex = 1; leftUnsortedIndex < static_cast<int32_t>(games.size()); leftUnsortedIndex
             ++) {
            // The key is the first unsorted element
            Game* key = games[leftUnsortedIndex];
            // Search for the appropriate key location with binary search. std::lower_bound required for sort stability
            // static_cast<int32_t> is fine, we won't have more than 500,000 items
            const int32_t targetKeyIndex = static_cast<int32_t>(std::lower_bound(games.begin(),
                                                                      games.begin() + leftUnsortedIndex, key,
                                                                      lessThanComparator) - games.begin());

            // Shift elements. Faster than linear shifting
            if (targetKeyIndex < leftUnsortedIndex) {
                // Shift everything after the key's index
                std::shift_right(games.begin() + targetKeyIndex, games.begin() + leftUnsortedIndex + 1, 1);
            }
            // Move the key to the correct position
            games[targetKeyIndex] = key;
        }
    }
}
