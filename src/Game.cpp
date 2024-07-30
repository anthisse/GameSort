#include "Game.hpp"

inline bool Game::compareTitles(const Game* const lhs, const Game* const rhs) {
    return (lhs->title_ < rhs->title_);
}

inline bool Game::compareScores(const Game* const lhs, const Game* const rhs) {
    return (lhs->score_ < rhs->score_);
}

inline bool Game::compareGenres(const Game* const lhs, const Game* const rhs) {
    return (lhs->genre_ < rhs->genre_);
}
