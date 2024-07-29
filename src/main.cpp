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

#include "Game.h"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "simdjson.h"
#include "timsort.h"
#include "buttons.h"
#include "TextureManager.h"

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {
    }
};

void handleCurl(const char* env_MobyKey);

void randomDataAnalysis();

int main() {
    ////// sfml /////
    sf::RenderWindow welcomeWindow(sf::VideoMode(800, 600), "GameSort", sf::Style::Close);
    welcomeWindow.setMouseCursorVisible(true);
    welcomeWindow.setKeyRepeatEnabled(true);

    Buttons arrowNextButton("arrowNext", (25 * 32) - 240, 32 * (16 + 0.5f));
    Buttons arrowPreviousButton("arrowPrevious", ((25 / 2.0f) * 32) - 32, 32 * (16 + 0.5f));

    sf::RectangleShape cursor;
    cursor.setSize(sf::Vector2f(2, 20));
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition((800 / 2.0f), (600 / 2.0f) - 45);

    while (welcomeWindow.isOpen()) {
        sf::Event Event;
        while (welcomeWindow.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed) {                      // Click X on the window
                welcomeWindow.close();
            }

            welcomeWindow.clear(sf::Color::White);
            welcomeWindow.draw(cursor);
            welcomeWindow.display();
        }
    }

    //// end sfml /////




        const char *env_MobyKey = std::getenv("MOBY_KEY");
        if (!env_MobyKey) {
            throw apiException();
        }

        // TODO terrible function name, refactor later
        // handleCurl(env_MobyKey);


        const std::string platformPath = "../platforms";
        try {
            std::filesystem::directory_iterator dirIt(platformPath);
            std::vector<std::filesystem::directory_entry> files;
            for (const auto &entry: dirIt) {
                std::ifstream file(entry.path());
                files.push_back(entry);
            }

            std::vector<Game *> games;
            for (const auto &entry: files) {
                std::cout << "file: " << entry.path().filename().string() << "\n";
            }
        }
        catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Filesystem error: " << e.what() << '\n';
        }

        randomDataAnalysis();
        return 0;
}




    void handleCurl(const char *env_MobyKey) {
        std::string url = "https://api.mobygames.com/v1/games?api_key=" + std::string(env_MobyKey);
        printf("url is %s", url.c_str());
        CURL *curl_handle;
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

    void randomDataAnalysis() {


        using std::chrono::duration_cast;
        using ms = std::chrono::milliseconds;
        using clock = std::chrono::steady_clock;

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution<float> reviewDistrib(1.0f, 5.0f);
        std::uniform_int_distribution<int> charDistrib('0', 'z');
        std::uniform_int_distribution<int> titleLengthDistrib(5, 25);
        std::vector<std::string> platforms = {"Nintendo Switch", "Xbox One X", "PlayStation 5", "PC", "macOS", "Linux"};
        std::uniform_int_distribution<int> platformLengthDistrib(1, 6);
        std::uniform_int_distribution<int> platformDistrib(0, 1);

        std::vector<Game *> data;
        for (int n = 0; n != 200000; ++n) {
            auto *game = new Game();
            game->reviewScore = reviewDistrib(generator);
            for (int i = 0; i < titleLengthDistrib(generator); i++) {
                char randomChar = static_cast<char>(charDistrib(generator));
                while ((randomChar >= ':' && randomChar <= '@') || (randomChar >= '[' && randomChar <= '`')) {
                    randomChar = static_cast<char>(charDistrib(generator));
                }
                game->title += randomChar;
            }
            data.push_back(game);
        }

        /*
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_real_distribution reviewDistrib(1.0f, 5.0f);
        std::uniform_int_distribution charDistrib('0', 'z');
        std::uniform_int_distribution titleLengthDistrib(5, 25);
        std::vector<std::string> platforms = {"Nintendo Switch", "Xbox One X", "PlayStation 5", "PC", "macOS", "Linux"};
        std::uniform_int_distribution platformLengthDistrib(1, 6);
        std::uniform_int_distribution platformDistrib(0, 1);

        std::vector<Game*> data;
        for (int n = 0; n != 200000; ++n) {
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
         */


        // TODO use timsort and merge sort instead of grailsort and std::stable_sort
        puts("Sorting by review score, then by title: ");

        puts("Tim sorting...");
        auto start = clock::now();
        ts::timsort(data, Game::compareTitles);
        ts::timsort(data, Game::compareScores);
        auto elapsedTime = duration_cast<ms>(clock::now() - start);
        std::cout << "Sorted array's first element:";
        std::cout << data.front()->reviewScore << '\n';
        std::cout << "Sorted array's last element: \n" << data.back()->reviewScore << '\n';
        std::cout << "Tim sort took " << elapsedTime.count() << " milliseconds.\n";

        std::shuffle(data.begin(), data.end(), generator);
        std::cout << "Unorted array's first element:";
        std::cout << data.front()->reviewScore << '\n';
        std::cout << "Unsorted array's last element: \n" << data.back()->reviewScore << '\n';

        // TODO use timsort and merge sort instead of grailsort and std::stable_sort
        start = clock::now();
        std::stable_sort(data.begin(), data.end(), Game::compareTitles);
        std::stable_sort(data.begin(), data.end(), Game::compareScores);
        elapsedTime = duration_cast<ms>(clock::now() - start);
        std::cout << "Sorted array's first element:";
        std::cout << data.front()->reviewScore << '\n';
        std::cout << "Sorted array's last element: \n" << data.back()->reviewScore << '\n';
        std::cout << "Stablesort took " << elapsedTime.count() << "milliseconds.\n";

        std::shuffle(data.begin(), data.end(), generator);
        std::cout << "Unorted array's first element:";
        std::cout << data.front()->reviewScore << '\n';
        std::cout << "Unsorted array's last element: \n" << data.back()->reviewScore << '\n';

        puts("Insertion sorting...");
        start = clock::now();
        ts::binaryInsertionSort(data, Game::compareTitles);
        ts::binaryInsertionSort(data, Game::compareScores);
        elapsedTime = duration_cast<ms>(clock::now() - start);
        std::cout << "Sorted array's first element:";
        std::cout << data.front()->reviewScore << '\n';
        std::cout << "Sorted array's last element: \n" << data.back()->reviewScore << '\n';
        std::cout << "Insertion sort took " << elapsedTime.count() << " milliseconds.\n";

        std::shuffle(data.begin(), data.end(), generator);
    }
