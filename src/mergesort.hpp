#pragma once
#include <vector>
#include "Game.hpp"

namespace ms {
    /**
     * @brief Wrapper for recursive merge sort implementation
     * @author Lana Daraiseh
     *
     * @param games An unsorted vector of pointers to Game objects.
     * @param comparator A pointer to a comparison function.
     *
     * Average computational complexity: O(n*log(n))
     * Average space complexity: O(n)
     * Stable algorithm
    */
    void mergeSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs));

    // merges 2 halves of the vector sorted previously; uses comparator to
    //			decide order of merging.
    /**
     * @brief Merges 2 individually sorted halves of a vector
     * @author Lana Daraiseh
     *
     * @param games A greater vector that needs to be merged together.
     * @param left The index of the first element of the left vector.
     * @param mid The index of the first element of the right vector.
     * @param right The index of the last element of the right vector.
     * @param comparator A pointer to a comparison function.
     *
     * This is a standard merge function that sews together two individually
     * sorted vectors into one sorted chunk. If one of the smaller vectors
     * runs out of elements early, the remaining elements of the other
     * smaller vector are simply copied over to the main vector without
     * any comparisons
     *
     * Average computational complexity: O(n + m)
     *     n = leftSlice's size and m = rightSlice's size
     * Average space complexity: O(n + m)
     *     n = leftSlice's size and m = rightSlice's size
     * Stable algorithm
     */
    void merge_(std::vector<Game*>& games, int left, int mid, int right,
                bool (*comparator)(const Game* lhs, const Game* rhs));

    /**
     * @brief Recursively divide the vector into halves until each segment length equals 1.
     * @author Lana Daraiseh
     *
     * @param games An unsorted vector of pointers to Game objects
     * @param left The index of the first element of the left vector.
     * @param right The index of the last element of the right vector
     * @param comparator A pointer to a comparison function.
     *
     * This function is the implementation of a standard top-down merge sort.
     * The base case here is when the left index is equal to the right index,
     * meaning the games vector size is one. While that isn't the case, the
     * function calculates it's midpoint from the left and right indices,
     * breaks the game vector into two halves, recursively calls itself
     * on the two halves and merges the result back together with the
     * merge function.
     * Average computational complexity: O(n * log(n))
     *     n = game vector's size
     * Average space complexity: O(n)
     *     n = game vector's size
     * Stable algorithm
     */
    void recursiveMergeSort_(std::vector<Game*>& games, int left, int right,
                             bool (*comparator)(const Game* lhs, const Game* rhs));
}
