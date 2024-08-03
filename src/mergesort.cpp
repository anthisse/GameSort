#include "mergesort.hpp"

#include <iostream>

namespace ms {
    void mergeSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // call recursive merge sort w/ entire game range
        recursiveMergeSort_(games, 0, games.size() - 1, comparator);
    }
    // recursivly divide the vector
    void recursiveMergeSort_(std::vector<Game*>& games, const size_t left, const size_t right,
                             bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // if segment is more than one
        if (left < right) {
            // calculate middle idx
            const size_t mid = left + (right - left) / 2;
            recursiveMergeSort_(games, left, mid, comparator); // sort left half
            recursiveMergeSort_(games, mid + 1, right, comparator); // sort right half
            merge_(games, left, mid, right, comparator); // merge the two halves
        }
    }

    void merge_(std::vector<Game*>& games, const size_t left, const size_t mid, const size_t right,
                bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // # of elements in the left array
        const size_t sizeLeft = mid - left + 1;

        // # of elements in the right array
        const size_t sizeRight = right - mid;

        // temp vectors for left + right arrays
        std::vector<Game*> leftGames(sizeLeft);
        std::vector<Game*> rightGames(sizeRight);

        // copy the data to the temp arrays
        for (size_t i = 0; i < sizeLeft; i++) {
            leftGames[i] = games[left + i];
        }
        for (size_t j = 0; j < sizeRight; j++) {
            rightGames[j] = games[mid + 1 + j];
        }

        // initial idx of left + right arrays
        size_t i = 0;
        size_t j = 0;

        // initial idx to start merging from
        size_t k = left;

        // merge temp arrays back in the original array
        while (i < sizeLeft && j < sizeRight) {
            if (comparator(leftGames[i], rightGames[j])) {
                games[k++] = leftGames[i++];
            } else {
                games[k++] = rightGames[j++];
            }
        }

        // JUST IN CASE - copy remaining elements of leftGames
        while (i < sizeLeft) {
            games[k++] = leftGames[i++];
        }

        // JUST IN CASE - copy remaining elements of rightGames
        while (j < sizeRight) {
            games[k++] = rightGames[j++];
        }
    }
}
