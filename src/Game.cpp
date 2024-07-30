#include "Game.hpp"

bool Game::compareTitles(const Game* const lhs, const Game* const rhs) {
    return (lhs->title < rhs->title);
}

bool Game::compareScores(const Game* const lhs, const Game* const rhs) {
    return (lhs->reviewScore < rhs->reviewScore);
}

bool Game::compareGenres(const Game* const lhs, const Game* const rhs) {
    return (lhs->genre < rhs->genre);
}
