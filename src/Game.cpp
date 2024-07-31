#include "Game.hpp"


Game::Game(std::string title, std::vector<std::string> genres, const double score, std::string platform) {
    this->title_ = std::move(title);
    this->genres = std::move(genres);
    this->score_ = score;
    this->platform = std::move(platform);
}

std::string Game::get_title() const {
    return this->title_;
}

std::vector<std::string> Game::get_genres() const {
    return this->genres;
}

double Game::get_score() const {
    return this->score_;
}

std::string Game::get_platform() const {
    return this->platform;
}

void Game::set_title(std::string title) {
    this->title_ = std::move(title);
}

void Game::set_genres(std::vector<std::string> genres) {
    this->genres = std::move(genres);
}

void Game::set_score(const double score) {
    this->score_ = score;
}

void Game::set_platform(std::string platform) {
    this->platform = std::move(platform);
}

bool Game::compareTitles(const Game* const lhs, const Game* const rhs) {
    return (lhs->title_ < rhs->title_);
}

bool Game::compareGenres(const Game* const lhs, const Game* const rhs) {
    return (lhs->platform < rhs->platform);
}

bool Game::compareScores(const Game* const lhs, const Game* const rhs) {
    return (lhs->score_ < rhs->score_);
}
