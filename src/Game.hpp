#pragma once

#include <string>
#include <vector>

class Game {
public:
    Game() = delete;

    Game(std::string title, std::vector<std::string> genres, double score, std::string platform);

    [[nodiscard("Getter")]] std::string get_title() const;

    [[nodiscard("Getter")]] std::vector<std::string> get_genres() const;

    [[nodiscard("Getter")]] double get_score() const;

    [[nodiscard("Getter")]] std::string get_platform() const;

    // Memory address is compared to resolve ties
    static bool compareTitles(const Game* lhs, const Game* rhs);

    static bool compareGenres(const Game* lhs, const Game* rhs);

    static bool compareScores(const Game* lhs, const Game* rhs);

    static bool comparePlatform(const Game* lhs, const Game* rhs);

private:
    std::string title_;
    std::vector<std::string> genres_;
    double score_ = 0.0F;
    std::string platform_;
};
