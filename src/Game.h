#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>
#include <vector>

// TODO Don't include if the game tag includes "adult" cause that's nasty
struct Game {
    std::vector<std::string> platforms;
    float reviewScore = 0.0f;
    std::string genre;
    std::string title;

    static bool compareTitles(const Game* lhs, const Game* rhs);

    static bool compareScores(const Game* lhs, const Game* rhs);

    static bool compareGenres(const Game* lhs, const Game* rhs);
};


#endif //GAMEDATA_H
