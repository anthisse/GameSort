#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// Libcurl C library. Fetch information from a URL.
#include <curl/curl.h>

// SFML. Graphics library.
#include <SFML/config.hpp>
#include <SFML/Graphics.hpp>

#include "Game.hpp"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "simdjson.h"
#include "timsort.hpp"

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {}
};

void handleCurl(const char* env_MobyKey);

void dataAnalysis(std::vector<Game*>& data);

std::vector<Game*> parseJsons();

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json);

bool isVulgar(const Game* game);

int main() {
    const char* env_MobyKey = std::getenv("MOBY_KEY");
    if (!env_MobyKey) {
        throw apiException();
    }

    // TODO terrible function name, refactor later
    // handleCurl(env_MobyKey);

    puts("about to parse jsons");
    std::vector<Game*> data = parseJsons();
    puts("In main, here is the data vector: ");
    dataAnalysis(data);
    return 0;
}

// Get blacklisted characters from csv
std::vector<std::string> getBlacklist() {
    const char* path = "../config/blacklist.csv";
    std::ifstream file(path);
    if (!file.is_open()) {
        std::string msg = "Failed to find blacklist file at ";
        msg.append(path);
        throw (std::ifstream::failure(msg));
    }
    std::vector<std::string> blacklist;
    std::string word;
    while (!file.eof()) {
        std::getline(file, word, ',');
        blacklist.push_back(word);
    }
    return blacklist;
}

std::vector<Game*> parseJsons() {
    using std::chrono::duration_cast;
    using ms = std::chrono::milliseconds;
    using clock = std::chrono::steady_clock;

    std::vector<std::string> blacklist = getBlacklist();
    auto start = clock::now();
    std::vector<Game*> games;
    // Iterate through each file and create Game objects
    const char* platformPath = "../games/platforms/jsons/";
    const std::filesystem::directory_iterator directoryIterator(platformPath);
    simdjson::ondemand::parser parser;

    for (const auto& entry : directoryIterator) {
        auto json = simdjson::padded_string::load(entry.path().string());
        simdjson::ondemand::document document = parser.iterate(json);
        for (auto game_json : document) {
            std::string title;
            if (game_json["title"].is_string()) {
                std::string_view titleView = game_json["title"].get_string();
                title = std::string(titleView);
            }
            std::vector<std::string> genres = getGenres(game_json);
            double score;
            if (game_json["moby_score"].is_null()) {
                score = 0.0F;
            } else {
                score = game_json["moby_score"].get_double();
            }
            std::string platform = entry.path().filename().replace_extension().string();
            Game* game = new Game(title, genres, score, platform);
            if (!isVulgar(game)) {
                games.push_back(game);
            }
        }
    }
    std::cout << "Finished parsing in " << duration_cast<ms>(clock::now() - start) << '\n';
    return games;
}

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json) {
    std::vector<std::string> genres;
    for (auto result : json["genres"]) {
        // Need to make sure that genres is a string before emplacing to avoid simdjson error
        if (result.is_string()) {
            std::string_view genreView = result.get_string();
            genres.emplace_back(genreView);
        }
    }
    return genres;
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

void print_stats(std::vector<Game*>& data) {
    std::cout << "Sorted array's first element: ";
    std::cout << "Title: " << data.front()->get_title();
    std::cout << ", Score: " << data.front()->get_score();
    std::cout << ", Genre: \n";
    for (const auto& genre : data.front()->get_genres()) {
        std::cout << '\t' << genre << '\n';
    }
    std::cout << "Platform: " << data.front()->get_platform();
    std::cout << '\n';
    std::cout << "\nSorted array's last element: ";
    std::cout << "Title: " << data.back()->get_title();
    std::cout << ", Score: " << data.back()->get_score();
    std::cout << ", Genre: \n";
    for (const auto& genre : data.back()->get_genres()) {
        std::cout << '\t' << genre << '\n';
    }
    std::cout << "Platform: " << data.back()->get_platform();
    std::cout << std::endl << std::endl;
}

void dataAnalysis(std::vector<Game*>& data) {
    using std::chrono::duration_cast;
    using ms = std::chrono::milliseconds;
    using clock = std::chrono::steady_clock;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::ranges::shuffle(data.begin(), data.end(), generator);
    puts("Sorting by title then score then platform:");
    puts("================================");

    puts("Tim sorting...");
    auto start = clock::now();
    ts::timsort(data, Game::compareTitles);
    ts::timsort(data, Game::compareScores);
    ts::timsort(data, Game::comparePlatform);

    auto elapsedTime = duration_cast<ms>(clock::now() - start);

    print_stats(data);
    std::cout << "Tim sort took " << elapsedTime.count() << " milliseconds.\n";

    puts("shuffling");
    std::shuffle(data.begin(), data.end(), generator);

    puts("============================");
    puts("stable_sort incoming");
    start = clock::now();
    std::stable_sort(data.begin(), data.end(), Game::compareTitles);
    std::stable_sort(data.begin(), data.end(), Game::compareScores);
    std::stable_sort(data.begin(), data.end(), Game::comparePlatform);
    elapsedTime = duration_cast<ms>(clock::now() - start);

    print_stats(data);
    std::cout << "Stablesort took " << elapsedTime.count() << "milliseconds.\n";

    puts("shuffling");
    std::shuffle(data.begin(), data.end(), generator);

    puts("==============================");
    puts("Insertion sorting...");
    start = clock::now();
    ts::binaryInsertionSort(data, Game::compareTitles);
    ts::binaryInsertionSort(data, Game::compareScores);
    ts::binaryInsertionSort(data, Game::comparePlatform);
    elapsedTime = duration_cast<ms>(clock::now() - start);
    print_stats(data);
    std::cout << "Insertion sort took " << elapsedTime.count() << " milliseconds.\n";
}

// Ignore games that are possibly offensive
bool isVulgar(const Game* game) {
    const std::vector<std::string> blacklist = getBlacklist();
    for (const auto& word : blacklist) {
        std::string lowerTitle = game->get_title();
        std::ranges::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), tolower);
        if (game->get_title().find(word) != std::string::npos) {
            return true;
        }
        // TODO fairly slow, replace with std::ranges::find
        for (const auto& genre : game->get_genres()) {
            if (genre == "Adult") {
                return true;
            }
        }
    }
    return false;
}
