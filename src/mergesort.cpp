#include "mergesort.hpp"

#include <iostream>

namespace ms {
    void mergeSort(std::vector<Game*>& games, bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // call recursive merge sort w/ entire game range
        _recursiveMergeSort(games, 0, games.size() - 1, comparator);
    }
    // recursivly divide the vector
    void _recursiveMergeSort(std::vector<Game*>& games, int left, int right,
                             bool (*comparator)(const Game* lhs, const Game* rhs)) {
        // if segment is more than one
        if (left < right) {
            // calculate middle idx
            int mid = left + (right - left) / 2;
            _recursiveMergeSort(games, left, mid, comparator); // sort left half
            _recursiveMergeSort(games, mid + 1, right, comparator); // sort right half
            _merge(games, left, mid, right, comparator); // merge the two halves
        }
    }

    void _merge(std::vector<Game*>& games, int left, int mid, int right,
                bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
        // # of elements in the left array
        int sizeLeft = mid - left + 1;

        // # of elements in the right array
        int sizeRight = right - mid;

        // temp vectors for left + right arrays
        std::vector<Game*> leftGames(sizeLeft);
        std::vector<Game*> rightGames(sizeRight);

        // copy the data to the temp arrays
        for (int i = 0; i < sizeLeft; i++) {
            leftGames[i] = games[left + i];
        }
        for (int j = 0; j < sizeRight; j++) {
            rightGames[j] = games[mid + 1 + j];
        }

        // initial idx of left + right arrays
        int i = 0;
        int j = 0;

        // initial idx to start merging from
        int k = left;

        // merge temp arrays back in the original array
        while (i < sizeLeft && j < sizeRight) {
            if (lessThanComparator(leftGames[i], rightGames[j])) {
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
