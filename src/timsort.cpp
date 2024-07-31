#include <algorithm>
#include <vector>

#include "timsort.hpp"

namespace ts {
    void binaryInsertionSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        for (ssize_t leftUnsorted = 1; leftUnsorted < static_cast<ssize_t>(games.size()); leftUnsorted++) {
            // The key is the first unsorted element
            Game* key = games[leftUnsorted];

            // Search for the appropriate key location with binary search using std::lower_bound for sort stability
            const ssize_t targetIndex = (std::lower_bound(games.begin(), games.begin() + leftUnsorted, key, comparator) - games.begin());

            // Shift elements to the right if necessary
            if (targetIndex < leftUnsorted) {
                std::shift_right(games.begin() + targetIndex, games.begin() + leftUnsorted + 1, 1);
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
                rightIndex++;
            }
            main_index++;
        }

        // Copy remaining elements of the vectors, if there are any
        while (leftIndex < leftSlice.size()) {
            mainVector[main_index] = leftSlice[leftIndex];
            leftIndex++;
            main_index++;
        }
        while (rightIndex < rightSlice.size()) {
            mainVector[main_index] = rightSlice[rightIndex];
            rightIndex++;
            main_index++;
        }
    }

    // Technically an introspective merge/insertion sort, but it's essentially a simplified timsort (no gallop)
    void timsort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // Minimum size of a run
        // Cast to ssize_t when using iterators to avoid narrowing conversion warnings when adding offsets
        // This is fine, since ssize_t is equal in size to long long int, which has an upper limit of 2^64 - 1
        constexpr ssize_t RUN_SIZE = 64;

        const ssize_t gameSize = static_cast<ssize_t>(games.size()); // NOLINT(*-use-auto)
        // Sort individual vector slices
        for (ssize_t i = 0; i < gameSize; i += RUN_SIZE) {
            std::vector slice(games.cbegin() + i, games.cbegin() + std::min(i + RUN_SIZE, gameSize));
            binaryInsertionSort(slice, comparator);
            // Use copy to quickly move the result back into the original vector
            std::ranges::copy(slice.begin(), slice.end(), games.begin() + i);
        }

        // Start merging the vectors
        for (ssize_t size = RUN_SIZE; size < gameSize; size *= 2) {
            for (ssize_t left = 0; left < gameSize; left += 2 * size) {
                const ssize_t mid = std::min(left + size, gameSize);
                const ssize_t right = std::min(left + 2 * size, gameSize);

                // Only start merging if the right slice is of non-zero size
                if (mid < right) {
                    // Create the slices to merge
                    std::vector leftSlice(games.cbegin() + left, games.cbegin() + mid);
                    std::vector rightSlice(games.cbegin() + mid, games.cbegin() + right);
                    std::vector<Game*> result(right - left);
                    merge_(leftSlice, rightSlice, result, comparator);
                    // Use copy to quickly move the result back into the original vector
                    std::ranges::copy(result.begin(), result.end(), games.begin() + left);
                }
            }
        }
    }
}
