#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// SFML. Graphics library.
#include <SFML/config.hpp>
#include <SFML/Graphics.hpp>

#include "Game.hpp"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "../lib/simdjson.h"


#include "timsort.hpp"
#include "buttons.h"
#include "TextureManager.h"
#include "Title.h"

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {}
};

void handleCurl(const char* env_MobyKey);

void dataAnalysis(std::vector<Game*>& data);

std::vector<Game*> parseJsons();

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json);

bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist);

int main(int argc, char** argv) {
    printf("program path: %s\n", argv[0]);
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

    // SFML
    sf::RenderWindow welcomeWindow(sf::VideoMode(1300, 700), "GameSort", sf::Style::Close);
    welcomeWindow.setMouseCursorVisible(true);
    welcomeWindow.setKeyRepeatEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        std::cout << "can't load font :(" << std::endl;
    }

    Text welcomeText("Game Sort", font, 35, sf::Text::Underlined, sf::Text::Bold, sf::Color::Blue,
                     sf::Vector2f(250 / 2.0f, (380 / 2.0f) - 150));

    //Buttons arrowNextButton("arrowNext", (25 * 45) - 180, 20 * (16 + 0.5f));
    //Buttons arrowPreviousButton("arrowPrevious", ((25 / 2.0f) * 5) - 32, 20 * (16 + 0.5f));

    sf::Texture& nextArrowTexture = TextureManager::getTexture("arrowNext");
    sf::Sprite nextArrowSprite;
    nextArrowSprite.setTexture(nextArrowTexture);
    nextArrowSprite.setScale(0.25, 0.25); // have to scale it down so it fits in the window bc big image
    nextArrowSprite.setOrigin(nextArrowTexture.getSize().x / 2.0f, nextArrowTexture.getSize().y / 2.0f);
    // set it somewhere we want on screen
    nextArrowSprite.setPosition(1250, 650);

    sf::Texture& previousArrowTexture = TextureManager::getTexture("arrowPrevious");
    sf::Sprite previousArrowSprite;
    previousArrowSprite.setTexture(previousArrowTexture);
    previousArrowSprite.setScale(0.25, 0.25); // have to scale it down so it fits in the window bc big image
    previousArrowSprite.setOrigin(previousArrowTexture.getSize().x / 2.0f, previousArrowTexture.getSize().y / 2.0f);
    // set it somewhere we want on screen
    previousArrowSprite.setPosition(1150, 650);

    bool pressedNextArrow = false;
    bool pressedPreviousArrow = false;

    while (welcomeWindow.isOpen()) {
        sf::Event Event;
        while (welcomeWindow.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed) {
                // Click X on the window
                welcomeWindow.close();
            } else if (Event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse; // 2-dimensional vector of floating point coordinates x,y
                mouse = sf::Mouse::getPosition(welcomeWindow);
                // Storing the values of where the event occurred in window

                if (nextArrowSprite.getGlobalBounds().contains(welcomeWindow.mapPixelToCoords(mouse))) {
                    pressedNextArrow = !pressedNextArrow;
                }
                if (previousArrowSprite.getGlobalBounds().contains(welcomeWindow.mapPixelToCoords(mouse))) {
                    pressedPreviousArrow = !pressedPreviousArrow;
                }
            }

            welcomeWindow.clear(sf::Color(200, 100, 50));
            welcomeWindow.draw(nextArrowSprite);
            welcomeWindow.draw(previousArrowSprite);
            welcomeWindow.draw(welcomeText.getText());

            if (pressedNextArrow) {
                // if forward arrow pressed, go forward in list of games
                welcomeWindow.clear();
            }

            if (pressedPreviousArrow) {
                // if back arrow pressed, go back in list of games
                welcomeWindow.clear();
            }

            welcomeWindow.display();
        }
    }

    // //// end sfml /////
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

    std::vector<std::string> blacklist;
    try {
        blacklist = getBlacklist();
    } catch (std::ifstream::failure& e) {
        std::cerr << e.what() << "\nblacklist not functional, config/blacklist.csv not found.\n";
    }
    const auto start = clock::now();
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
            const std::vector<std::string> genres = getGenres(game_json);
            double score;
            if (game_json["moby_score"].is_null()) {
                score = 0.0F;
            } else {
                score = game_json["moby_score"].get_double();
            }
            const std::string platform = entry.path().filename().replace_extension().string();
            auto game = new Game(title, genres, score, platform);
            if (blacklist.empty() || !isBlacklisted(game, blacklist)) {
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

void print_stats(const std::vector<Game*>& data) {
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
    std::cout << "data size: " << data.size() << '\n';
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
    ts::timsort(data, Game::compareScores);
    ts::timsort(data, Game::comparePlatform);
    ts::timsort(data, Game::compareTitles);

    auto elapsedTime = duration_cast<ms>(clock::now() - start);

    print_stats(data);
    std::cout << "Tim sort took " << elapsedTime.count() << " milliseconds.\n";

    puts("shuffling");
    generator();
    std::ranges::shuffle(data.begin(), data.end(), generator);

    puts("============================");
    puts("stable_sort incoming");
    start = clock::now();
    std::ranges::stable_sort(data.begin(), data.end(), Game::compareScores);
    std::ranges::stable_sort(data.begin(), data.end(), Game::comparePlatform);
    std::ranges::stable_sort(data.begin(), data.end(), Game::compareTitles);
    elapsedTime = duration_cast<ms>(clock::now() - start);

    print_stats(data);
    std::cout << "Stablesort took " << elapsedTime.count() << "milliseconds.\n";

    puts("shuffling");
    generator();
    std::ranges::shuffle(data.begin(), data.end(), generator);

    puts("==============================");
    puts("Insertion sorting...");
    start = clock::now();
    ts::binaryInsertionSort(data, Game::compareScores);
    ts::binaryInsertionSort(data, Game::comparePlatform);
    ts::binaryInsertionSort(data, Game::compareTitles);
    elapsedTime = duration_cast<ms>(clock::now() - start);
    print_stats(data);
    std::cout << "Insertion sort took " << elapsedTime.count() << " milliseconds.\n";
}

// Ignore games that are possibly offensive
bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist) {
    std::string lowerTitle = game->get_title();
    std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
    for (const auto& word : blacklist) {
        if (game->get_title().find(word) != std::string::npos) {
            return true;
        }
        std::vector<std::string> genres = game->get_genres();
        if (std::ranges::find(genres.begin(), genres.end(), "Adult") != genres.end()) {
            return true;
        }
    }
    return false;
}
