#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

// SFML. Graphics library.
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

#include "Game.hpp"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "../lib/simdjson.h"
#include "timsort.hpp"
#include "buttons.h"
#include "mergesort.h"
#include "TextureManager.h"
#include "Title.h"

class apiException final : public std::runtime_error {
public:
    apiException() : std::runtime_error("Moby API key missing") {}
};

void dataAnalysis(std::vector<Game*>& data);

std::vector<Game*> parseJsons();

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json);

bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist);

sf::Sprite getSprite(sf::Texture& texture, float xPos, float yPos, float xScale, float yScale) {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    // Scale the sprite so that it fits nicely on the screen and set its position
    sprite.setScale(xScale, yScale);
    sprite.setOrigin(static_cast<float>(texture.getSize().x) / 2.0f, static_cast<float>(texture.getSize().y) / 2.0f);
    sprite.setPosition(xPos, yPos);
    return sprite;
}

void renderMainWindow(const sf::Font& font) {
    sf::Color gatorOrange(250, 70, 22);
    // SFML
    sf::RenderWindow mainWindow(sf::VideoMode(1300, 700), "GameSort", sf::Style::Close);
    mainWindow.setMouseCursorVisible(true);
    mainWindow.setKeyRepeatEnabled(true);
    TextureManager* textureManager = TextureManager::getInstance("../res");
    Text welcomeText("Game Sort", font, 35, sf::Text::Underlined, sf::Text::Bold, sf::Color::White,
                     sf::Vector2f(250 / 2.0f, (380 / 2.0f) - 150));
    Text sortGamesText("Order by:", font, 28, sf::Text::Underlined, sf::Text::Bold, gatorOrange,
                       sf::Vector2f(1100, (420 / 2.0f) - 150));


    sf::Sprite nextArrow = getSprite(textureManager->getTexture("arrowNext"), 1155, 650, 0.25, 0.25);
    sf::Sprite prevArrow = getSprite(textureManager->getTexture("arrowPrevious"), 1055, 650, 0.25, 0.25);
    sf::Sprite title = getSprite(textureManager->getTexture("titleButton"), 1100, 145, 0.13, 0.13);
    sf::Sprite rating = getSprite(textureManager->getTexture("ratingButton"), 1100, 245, 0.13, 0.13);
    sf::Sprite genre = getSprite(textureManager->getTexture("genreButton"), 1100, 345, 0.13, 0.13);
    sf::Sprite platform = getSprite(textureManager->getTexture("platformButton"), 1100, 445, 0.13, 0.13);
    // bool for these so we can toggle them later on in main for when button is pressed
    while (mainWindow.isOpen()) {
        sf::Event Event{};
        while (mainWindow.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed) {
                // Click X on the window
                // data.clear();
                mainWindow.close();
                std::exit(0);
            }
            if (Event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse; // 2-dimensional vector of floating point coordinates x,y
                mouse = sf::Mouse::getPosition(mainWindow);
                if (nextArrow.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    // TODO go to next page
                    puts("next arr press, goto next page");
                }
                if (prevArrow.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    // todo go to prev page
                    puts("prev arr press, goto prev page");
                }
                if (title.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("title pressed, sort by title");
                }
                if (rating.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("rating pressed, sort by rating");
                }
                if (genre.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("genre pressed, sort by genre");
                }
                if (platform.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("platform pressed, sort by platform");
                }
            }


            mainWindow.clear(sf::Color(250, 70, 22));
            mainWindow.draw(title);
            mainWindow.draw(rating);
            mainWindow.draw(genre);
            mainWindow.draw(platform);
            mainWindow.draw(nextArrow);
            mainWindow.draw(prevArrow);
            mainWindow.draw(welcomeText.getText());
            mainWindow.draw(sortGamesText.getText());
            mainWindow.display();
        }
        // Lock framerate to 60 to avoid high CPU consumption
        sf::sleep(sf::seconds(1.0F / 60.0F));
    }
}

std::vector<Game*> loadGames(const sf::Font& font) {
    sf::RenderWindow loadingWindow(sf::VideoMode(900, 450), "GameSort", sf::Style::Close);
    loadingWindow.setMouseCursorVisible(true);
    sf::Text text;
    text.setString("Parsing jsons...");
    text.setFont(font);
    text.setCharacterSize(50);
    text.setStyle(sf::Text::Bold | sf::Text::Italic);
    text.setFillColor(sf::Color::White);

    const sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(static_cast<float>(loadingWindow.getSize().x) / 2.0f,
                     static_cast<float>(loadingWindow.getSize().y) / 2.0f);
    loadingWindow.clear(sf::Color(0, 33, 165));
    loadingWindow.draw(text);
    loadingWindow.display();
    std::vector<Game*> games = parseJsons();
    loadingWindow.close();
    return games;
}

int main(int argc, char** argv) {
    printf("program path: %s\n", argv[0]);
    const char* env_MobyKey = std::getenv("MOBY_KEY");
    if (!env_MobyKey) {
        throw apiException();
    }

    sf::Font font;
    if (!font.loadFromFile("../res/font.ttf")) {
        throw (std::runtime_error("unable to load font, aborting!"));
    }

    std::vector<Game*> games = loadGames(font);

    renderMainWindow(font);
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
    std::cout << "Finished parsing in " << duration_cast<ms>(clock::now() - start).count() << '\n';
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
    using millis = std::chrono::milliseconds;
    using clock = std::chrono::steady_clock;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::ranges::shuffle(data.begin(), data.end(), generator);
    puts("Sorting by title then score then platform:");
    puts("================================");

    puts("Merge sorting...");
    auto start = clock::now();
    ms::mergeSort(data, Game::compareScores);
    ms::mergeSort(data, Game::comparePlatform);
    ms::mergeSort(data, Game::compareTitles);
    auto elapsedTime = duration_cast<millis>(clock::now() - start);
    print_stats(data);
    printf("Merge sort took %lld millisconds.\n", elapsedTime.count());

    puts("Shuffling");
    generator();
    std::ranges::shuffle(data.begin(), data.end(), generator);

    puts("Tim sorting...");
    start = clock::now();
    ts::timsort(data, Game::compareScores);
    ts::timsort(data, Game::comparePlatform);
    ts::timsort(data, Game::compareTitles);

    elapsedTime = duration_cast<millis>(clock::now() - start);

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
    elapsedTime = duration_cast<millis>(clock::now() - start);

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
    elapsedTime = duration_cast<millis>(clock::now() - start);
    print_stats(data);
    std::cout << "Insertion sort took " << elapsedTime.count() << " milliseconds.\n";
}

// Ignore games that are possibly offensive
bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist) {
    std::string lowerTitle = game->get_title();
    std::ranges::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
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
