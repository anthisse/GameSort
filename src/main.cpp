#include <algorithm>
#include <chrono>
#include <string>
#include <random>
#include <iostream>

// Grailsort implementation provided by Morwenn.
// Original code at https://github.com/HolyGrailSortProject/Rewritten-Grailsort
#include "grailsort.h"

struct GameData {
    std::string title;
    std::string genre;
    std::vector<std::string> platforms;
    float reviewScore;

    static inline bool compareTitles(GameData* const lhs, GameData* const rhs) {
        return (lhs->title < rhs->title);
    }

    static inline bool compareScores(GameData* const lhs, GameData* const rhs) {
        return (lhs->reviewScore < rhs->reviewScore);
    }
};


int main() {
    using namespace std::chrono;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<float> reviewDistrib(1, 5);
    std::uniform_int_distribution<char> charDistrib('0', 'z');
    std::uniform_int_distribution<int> titleLengthDistrib(5, 25);
    std::vector<std::string> platforms = {"Nintendo Switch", "Xbox One X", "PlayStation 5", "PC", "macOS", "Linux"};
    std::uniform_int_distribution<int> platformLengthDistrib(1, 6);
    std::uniform_int_distribution<int> platformDistrib(0, 1);

    std::vector<GameData*> data;
    for (int n = 0; n != 100000; ++n) {
        auto* game = new GameData();
        game->reviewScore = reviewDistrib(generator);
        for (int i = 0; i < titleLengthDistrib(generator); i++) {
            char randomChar = charDistrib(generator);
            while ( (randomChar >= ':' && randomChar <= '@') || (randomChar >= '[' && randomChar <= '`') ) {
                randomChar = charDistrib(generator);
            }
            game->title += randomChar;
        }
        data.push_back(game);
    }
    puts("Sorting by review score, then by title: ");
    auto start = high_resolution_clock::now();
    std::stable_sort(data.begin(), data.end(), GameData::compareTitles);
    std::stable_sort(data.begin(), data.end(), GameData::compareScores);
    auto elapsedTime = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "data: \n";
    std::cout << "This took " << elapsedTime.count() << " milliseconds.\n";

    std::shuffle(data.begin(), data.end(), generator);

    std::cout << "data: \n";
    start = high_resolution_clock::now();
    grailsort(data.begin(), data.end(), GameData::compareTitles);
    grailsort(data.begin(), data.end(), GameData::compareScores);
    elapsedTime = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    std::cout << "This took " << elapsedTime.count() << " milliseconds.\n";

    std::cout << "first element " << data[0]->title << '\n';
    return 0;
}
