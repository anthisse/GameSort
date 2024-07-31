#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

// TODO Blacklist games that have certain NSFW keywords in their genres and titles
class Game {
public:
    Game() = default;

    Game(std::string title, std::vector<std::string> genres, double score, std::string platform);

    [[nodiscard("Getter")]] std::string get_title() const;

    void set_title(std::string title);

    [[nodiscard("Getter")]] std::vector<std::string> get_genres() const;

    void set_genres(std::vector<std::string> genres);

    [[nodiscard("Getter")]] double get_score() const;

    void set_score(double score);

    [[nodiscard("Getter")]] std::string get_platform() const;

    void set_platform(std::string platform);

    static bool compareTitles(const Game* lhs, const Game* rhs);

    static bool compareScores(const Game* lhs, const Game* rhs);

    static bool compareGenres(const Game* lhs, const Game* rhs);

private:
    std::string title_;
    std::vector<std::string> genres;
    double score_ = 0.0F;
    std::string platform;
};


#endif //GAME_H
