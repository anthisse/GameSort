#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <curl/curl.h>
#include <SFML/config.hpp>
#include <SFML/Graphics.hpp>

// Grailsort implementation provided by Morwenn.
// Original code at https://github.com/HolyGrailSortProject/Rewritten-Grailsort
#include "grailsort.h"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "simdjson.h"

struct GameData {
    std::string title;
    std::string genre;
    std::vector<std::string> platforms;
    float reviewScore = 0;

    static bool compareTitles(const GameData *const lhs, const GameData *const rhs) {
        return (lhs->title < rhs->title);
    }

    static bool compareScores(const GameData *const lhs, const GameData *const rhs) {
        return (lhs->reviewScore < rhs->reviewScore);
    }
};

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {
    }
};

int main() {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        puts("1");
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
        /* example.com is redirected, so we tell libcurl to follow redirection */
        puts("2");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res gets the return code */
        puts("3");
        res = curl_easy_perform(curl);
        /* Check for errors */
        puts("4");
        if (res != CURLE_OK) {
            puts("5");
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        puts("6");
        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    const char *env_MobyKey = std::getenv("MOBY_KEY");
    if (!env_MobyKey) {
        throw apiException();
    }

    using std::chrono::duration_cast, std::chrono::milliseconds;
    typedef std::chrono::steady_clock clock;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution reviewDistrib(1.0f, 5.0f);
    std::uniform_int_distribution charDistrib('0', 'z');
    std::uniform_int_distribution titleLengthDistrib(5, 25);
    std::vector<std::string> platforms = {"Nintendo Switch", "Xbox One X", "PlayStation 5", "PC", "macOS", "Linux"};
    std::uniform_int_distribution platformLengthDistrib(1, 6);
    std::uniform_int_distribution platformDistrib(0, 1);

    std::vector<GameData *> data;
    for (int n = 0; n != 100000; ++n) {
        auto *game = new GameData();
        game->reviewScore = reviewDistrib(generator);
        for (int i = 0; i < titleLengthDistrib(generator); i++) {
            char randomChar = charDistrib(generator);
            while ((randomChar >= ':' && randomChar <= '@') || (randomChar >= '[' && randomChar <= '`')) {
                randomChar = charDistrib(generator);
            }
            game->title += randomChar;
        }
        data.push_back(game);
    }
    puts("Sorting by review score, then by title: ");
    auto start = clock::now();
    grailsort(data.begin(), data.end(), GameData::compareTitles);
    grailsort(data.begin(), data.end(), GameData::compareScores);
    auto elapsedTime = duration_cast<milliseconds>(clock::now() - start);
    std::cout << "This took " << elapsedTime.count() << " milliseconds.\n";

    std::cout << "first element " << data[0]->title << '\n';
    return 0;
}
