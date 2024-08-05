#include <algorithm>
#include <vector>

#include "timsort.hpp"

namespace ts {
    // All instances of static_cast<long long> are fine, as our data will never be larger than 500,000 elements.
    // long long can represent at least 9,223,372,036,854,775,807.
    void binaryInsertionSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        for (size_t leftUnsorted = 1; leftUnsorted < games.size(); leftUnsorted++) {
            // The key is the first unsorted element
            Game* key = games[leftUnsorted];

            // Search for the appropriate key location with binary search using std::lower_bound for sort stability
            const size_t targetIndex = (std::lower_bound(games.begin(),
                                                         games.begin() + static_cast<long long>(leftUnsorted), key,
                                                         comparator) - games.begin());

            // Shift elements to the right if necessary
            if (targetIndex < leftUnsorted) {
                std::shift_right(games.begin() + static_cast<long long>(targetIndex), games.begin() +
                                     static_cast<long long>(leftUnsorted) + 1, 1);
            }

            // Move the key to the correct position
            games[targetIndex] = key;
        }
    }

    void merge_(const std::vector<Game*>& leftSlice, const std::vector<Game*>& rightSlice,
                std::vector<Game*>& mainVector, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        size_t leftIndex = 0, rightIndex = 0, main_index = 0;
        // While inside the bounds of the split vectors
        while (leftIndex < leftSlice.size() && rightIndex < rightSlice.size()) {
            // Compare element-by-element and overwrite the appropriate position in the main vector
            if (comparator(leftSlice[leftIndex], rightSlice[rightIndex])) {
                mainVector[main_index] = leftSlice[leftIndex];
                leftIndex++;
            } else {
                mainVector[main_index] = rightSlice[rightIndex];
                ++rightIndex;
            }
            ++main_index;
        }

        // Copy remaining elements of the vectors, if there are any
        while (leftIndex < leftSlice.size()) {
            mainVector[main_index] = leftSlice[leftIndex];
            ++leftIndex;
            ++main_index;
        }
        while (rightIndex < rightSlice.size()) {
            mainVector[main_index] = rightSlice[rightIndex];
            ++rightIndex;
            ++main_index;
        }
    }

    // Technically an introspective merge/insertion sort, but it's essentially a simplified timsort (no gallop)
    void timsort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // Minimum size of a run. 512 is an arbitrary value, but it seems to work well
        // Cast to size_t when using iterators to resolve narrowing conversion warnings when adding offsets
        // This is fine, since size_t is equal in size to long long int
        constexpr size_t RUN_SIZE = 512;

        const size_t gameSize = static_cast<size_t>(games.size()); // NOLINT(*-use-auto)
        // Sort individual vector slices
        for (size_t i = 0; i < gameSize; i += RUN_SIZE) {
            std::vector slice(games.cbegin() + static_cast<long long>(i),
                              games.cbegin() + static_cast<long long>(std::min(i + RUN_SIZE, gameSize)));
            binaryInsertionSort(slice, comparator);
            // Use copy to quickly move the result back into the original vector
            std::ranges::copy(slice.begin(), slice.end(), games.begin() + static_cast<long long>(i));
        }

        // Start merging the vectors
        for (size_t size = RUN_SIZE; size < gameSize; size *= 2) {
            for (size_t left = 0; left < gameSize; left += 2 * size) {
                const size_t mid = std::min(left + size, gameSize);
                const size_t right = std::min(left + 2 * size, gameSize);

                // Only start merging if the right slice is of non-zero size
                if (mid < right) {
                    // Create the slices to merge
                    std::vector leftSlice(games.cbegin() + static_cast<long long>(left),
                                          games.cbegin() + static_cast<long long>(mid));
                    std::vector rightSlice(games.cbegin() + static_cast<long long>(mid),
                                           games.cbegin() + static_cast<long long>(right));
                    std::vector<Game*> result(right - left);
                    merge_(leftSlice, rightSlice, result, comparator);
                    // Use copy to quickly move the result back into the original vector
                    std::ranges::copy(result.begin(), result.end(), games.begin() + static_cast<long long>(left));
                }
            }
        }
    }
}
