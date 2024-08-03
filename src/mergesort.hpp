#pragma once
#include <vector>
#include "Game.hpp"

namespace ms {
	// this functions starts the recursive merge sort
	void mergeSort(std::vector<Game*>& games, bool(*comparator)(const Game* lhs, const Game* rhs));

	// merges 2 halves of the vector sorted previously; uses comparator to 
	//			decide order of merging.
	void _merge(std::vector<Game*>& games, int left, int mid, int right,
		bool (*lessThanComparator)(const Game* lhs, const Game* rhs));

	// recursively divides the vector into halves until each segment length = 1
	void _recursiveMergeSort(std::vector<Game*>& games, int left, int right,
		bool(*comparator)(const Game* lhs, const Game* rhs));
}
