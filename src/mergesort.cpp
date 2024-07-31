#include "mergesort.h"

#include <iostream>
using namespace std;

namespace ms {

	void mergeSort(vector<Game*>& games, bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
		// call recursive merge sort w/ entire game range
		_recursiveMergeSort(games, 0, games.size() - 1, lessThanComparator);

	}

	// recursivly divide the vector 
	void _recursiveMergeSort(vector<Game*>& games, int left, int right, bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
		// if segment is more than one
		if (left < right) {
			// calculate middle idx
			int mid = left + (right - left) / 2;
			_recursiveMergeSort(games, left, mid, lessThanComparator); // sort left half
			_recursiveMergeSort(games, mid + 1, right, lessThanComparator); // sort right half
			_merge(games, left, mid, right, lessThanComparator); // merge the two halves
		}

	}


	void _merge(vector<Game*>& games, int left, int mid, int right, bool (*lessThanComparator)(const Game* lhs, const Game* rhs)) {
		
		// # of elements in the left array
		int sizeLeft = mid - left + 1;

		// # of elements in the right array
		int sizeRight = right - mid;

		// temp vectors for left + right arrays
		vector<Game*> leftGames(sizeLeft);
		vector<Game*> rightGames(sizeRight);

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
			}
			else {
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