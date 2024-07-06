#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <curl/curl.h>
#include <SFML/config.hpp>
#include <SFML/Graphics.hpp>

#include "Game.h"

// Grailsort implementation provided by Morwenn.
// Original code at https://github.com/HolyGrailSortProject/Rewritten-Grailsort
#include "grailsort.h"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "simdjson.h"
#include "timsort.h"

// FIXME Only platforms with over 1000 games included for now


// TODO use timsort and merge sort instead of grailsort and std::stable_sort

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {
    }
};

void handleCurl(const char* env_MobyKey);

int main() {
    const char* env_MobyKey = std::getenv("MOBY_KEY");
    if (!env_MobyKey) {
        throw apiException();
    }

    // TODO terrible function name, refactor later
    // handleCurl(env_MobyKey);

    using std::chrono::duration_cast;
    typedef std::chrono::milliseconds millis;
    typedef std::chrono::steady_clock clock;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution reviewDistrib(1.0f, 5.0f);
    std::uniform_int_distribution charDistrib('0', 'z');
    std::uniform_int_distribution titleLengthDistrib(5, 25);
    std::vector<std::string> platforms = {"Nintendo Switch", "Xbox One X", "PlayStation 5", "PC", "macOS", "Linux"};
    std::uniform_int_distribution platformLengthDistrib(1, 6);
    std::uniform_int_distribution platformDistrib(0, 1);

    std::vector<Game*> data;
    for (int n = 0; n != 100; ++n) {
        auto* game = new Game();
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

    // TODO use timsort and merge sort instead of grailsort and std::stable_sort
    // puts("Sorting by review score, then by title: ");
    // auto start = clock::now();
    // grailsort(data.begin(), data.end(), Game::compareTitles);
    // grailsort(data.begin(), data.end(), Game::compareScores);
    // auto elapsedTime = duration_cast<millis>(clock::now() - start);
    // std::cout << "This took " << elapsedTime.count() << " milliseconds.\n";
    //
    // std::shuffle(data.begin(), data.end(), generator);

    // TODO use timsort and merge sort instead of grailsort and std::stable_sort
    // auto start = clock::now();
    // std::stable_sort(data.begin(), data.end(), Game::compareTitles);
    // std::stable_sort(data.begin(), data.end(), Game::compareScores);
    // auto elapsedTime = duration_cast<millis>(clock::now() - start);

    puts("Insertion sorting...");
    ts::_insertionSort(data, Game::compareScores);
    std::cout << "Sorted array:\n";
    for (const auto& elem : data) {
        std::cout << elem->reviewScore << "\n";
    }
    std::cout << std::endl;

    return 0;
}

void handleCurl(const char* env_MobyKey) {
    std::string url = "https://api.mobygames.com/v1/games?api_key=" + std::string(env_MobyKey);
    printf("url is %s", url.c_str());
    CURL* curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res gets the return code */
        res = curl_easy_perform(curl_handle);
        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl_handle);
    }
}
