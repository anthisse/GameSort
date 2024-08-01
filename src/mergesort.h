#pragma once
#include <vector>
#include "Game.hpp"

#include <iostream>

namespace ms {
	// this functions starts the recursive merge sort
	void mergeSort(std::vector<Game*>& games, bool(*lessThanComparator)
		(const Game* lhs, const Game* rhs));

	// merges 2 halves of the vector sorted previously; uses comparator to 
	//			decide order of merging.
	void _merge(std::vector<Game*>& games, int left, int mid, int right,
		bool (*lessThanComparator)(const Game* lhs, const Game* rhs));

	// recursively divides the vector into halves until each segment length = 1
	void _recursiveMergeSort(std::vector<Game*>& games, int left, int right,
		bool(*lessThanComparator)(const Game* lhs, const Game* rhs));
}
