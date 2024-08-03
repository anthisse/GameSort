#pragma once

#include <vector>
#include "Game.hpp"

namespace ts {
    /**
     * @brief Binary insertion sort implementation.
     * @author Anthony Thisse
     *
     * @param games An unsorted vector of pointers to Game objects
     * @param comparator A pointer to a comparison function.
     *
     * Insertion sort is rather slow on containers with many elements.
     * By using binary search instead of a linear search to find the correct
     * positions of unsorted elements, the searching process is siginificantly
     * improved from a computational complexity of O(n) to O(log(n)).
     * This makes the sort reasonably fast on its own. Note that
     * std::lower_bound is a C++ built-in implementation of binary search.
     *
     * However, binary search does not improve the overall computational
     * computational complexity of the sorting algorithm,
     * as elements still need to be linearly shifted to the right.
     *
     * This function is used in the final timsort algorithm as a way to
     * sort small slices of a std::vector with little overhead.
     *
     * Average computational complexity: O(n^2)
     *     n = games vector's size
     * Average space complexity: O(1)
     * Stable algorithm
    */
    void binaryInsertionSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs));

    /**
     * @brief Merges two sorted vectors into a bigger vector
     * @author Anthony Thisse
     * @author Adapted from GeeksForGeeks: C++ Program For Merge Sort
     * https://www.geeksforgeeks.org/cpp-program-for-merge-sort/
     *
     * @param leftSlice The sorted left half of the main vector to merge
     * @param rightSlice The sorted right half of the main vector to merge
     * @param mainVector A greater vector that needs to be merged together
     * @param comparator A pointer to a comparison function
     *
     * This function compares the elements of the left and right slices
     * element-by-element and builds a sorted list of the elements.
     * If one slice runs out of elements early, the remaining elements
     * of the other slice are copied to the main vector with no comparisons.
     *
     * This is used in the final timsort algorithm to sew together the
     * small slices that were sorted by binaryInsertionSort.
     *
     * Average computational complexity: O(n + m)
     *     n = leftSlice's size and m = rightSlice's size
     * Average space complexity: O(n + m)
     *     n = leftSlice's size and m = rightSlice's size
     * Stable algorithm
     */
    void merge_(const std::vector<Game*>& leftSlice, const std::vector<Game*>& rightSlice,
                std::vector<Game*>& mainVector, bool (*comparator)(const Game* lhs, const Game* rhs));

    /**
     * @brief A simplified implementation of timsort
     * @author Anthony Thisse
     * @author Adapted from GeeksForGeeks: Tim Sort in Python
     * https://www.geeksforgeeks.org/tim-sort-in-python/
     * @param games An unsorted vector of pointers to Game objects
     * @param comparator A pointer to a comparison function
     *
     * This function is strictly speaking, not timsort. It is instead a hybrid
     * algorithm that combines merge sort and binary insertion sort.
     * This means that the "gallop" step that a typical timsort implementation
     * would use is not present. Thus, this is probably more accurately named,
     * "introspective merge sort."
     *
     * The function slices the games vector into 512 element "slices" and
     * uses binary insertion sort to quickly sort each of them. It then
     * iteratively merges each of the slices together to obtain a sorted
     * vector of game pointers.
     *
     * This is tightly coupled to std::vector. To fix this, the algorithm
     * needs to be rewritten to use iterators, similar to how std::sort
     * and std::stable_sort work.
     *
     * Average computational complexity: O(n log(n))
     *     n = games vector's size
     * The log(n) comes from the repeated division of the main vector
     * Average space complexity: O(n) (slices duplicated from main vector)
     * Stable algorithm
     */
    void timsort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs));
}
