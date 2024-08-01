#include "Game.hpp"

#include <algorithm>

Game::Game(std::string title, std::vector<std::string> genres, const double score, std::string platform) {
    this->title_ = std::move(title);
    this->genres_ = std::move(genres);
    this->score_ = score;
    this->platform_ = std::move(platform);
}

std::string Game::get_title() const {
    return this->title_;
}

std::vector<std::string> Game::get_genres() const {
    return this->genres_;
}

double Game::get_score() const {
    return this->score_;
}

std::string Game::get_platform() const {
    return this->platform_;
}

void Game::set_title(std::string title) {
    this->title_ = std::move(title);
}

void Game::set_genres(std::vector<std::string> genres) {
    this->genres_ = std::move(genres);
}

void Game::set_score(const double score) {
    this->score_ = score;
}

void Game::set_platform(std::string platform) {
    this->platform_ = std::move(platform);
}

// Enforce stability by comparing the platforms if the titles are the same
// Technically, this won't work if both fields are the same, but our data shouldn't have duplicate games anyway
bool Game::compareTitles(const Game* lhs, const Game* rhs) {
    if (lhs->title_ == rhs->title_) {
        return lhs->platform_ < rhs->platform_;
    }
    return lhs->title_ < rhs->title_;
}

// For the rest of the comparisons, use the title as a tie-breaker
bool Game::compareGenres(const Game* const lhs, const Game* const rhs) {
    if (lhs->genres_ == rhs->genres_) {
        return lhs->title_ < rhs->title_;
    }
    return lhs->genres_ < rhs->genres_;
}

bool Game::comparePlatform(const Game* const lhs, const Game* const rhs) {
    if (lhs->platform_ == rhs->platform_) {
        return lhs->title_ < rhs->title_;
    }
    // Convert to lowercase first to make the comparison case-insensitive
    std::string leftLowerPlatform = lhs->platform_;
    std::string rightLowerPlatform = rhs->platform_;
    std::ranges::transform(leftLowerPlatform.begin(), leftLowerPlatform.end(), leftLowerPlatform.begin(), tolower);
    std::ranges::transform(rightLowerPlatform.begin(), rightLowerPlatform.end(), rightLowerPlatform.begin(), tolower);
    return (leftLowerPlatform < rightLowerPlatform);
}

bool Game::compareScores(const Game* const lhs, const Game* const rhs) {
    if (lhs->score_ == rhs->score_) {
        return lhs->title_ < rhs->title_;
    }
    return (lhs->score_ < rhs->score_);
}
