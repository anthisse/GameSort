#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

// TODO Blacklist games that have certain NSFW keywords in their genres and titles
struct Game {
    std::vector<std::string> platforms;
    float reviewScore = 0.0F;
    std::string genre;
    std::string title;

    static bool compareTitles(const Game* lhs, const Game* rhs);

    static bool compareScores(const Game* lhs, const Game* rhs);

    static bool compareGenres(const Game* lhs, const Game* rhs);
};


#endif //GAME_H
