#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

// TODO Blacklist games that have certain NSFW keywords in their genres and titles
class Game {
public:

    static bool compareTitles(const Game* lhs, const Game* rhs);

    static bool compareScores(const Game* lhs, const Game* rhs);

    static bool compareGenres(const Game* lhs, const Game* rhs);

private:
    std::vector<std::string> platforms_;
    float score_ = 0.0F;
    std::string genre_;
    std::string title_;
};


#endif //GAME_H
