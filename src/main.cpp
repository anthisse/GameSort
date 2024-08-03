#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// SFML. Graphics library.
#include <SFML/Graphics.hpp>

#include "Game.hpp"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "../lib/simdjson.h"
#include "timsort.hpp"
#include "mergesort.h"
#include "TextureManager.h"
#include "Title.h"

void dataAnalysis(std::vector<Game*>& data);

std::vector<Game*> parseJsons();

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json);

bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist);

sf::Sprite getSprite(const sf::Texture& texture, const float xPos, float const yPos,
    float const xScale, float const yScale) {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    // Scale the sprite so that it fits nicely on the screen and set its position
    sprite.setScale(xScale, yScale);
    sprite.setOrigin(static_cast<float>(texture.getSize().x) / 2.0f, static_cast<float>(texture.getSize().y) / 2.0f);
    sprite.setPosition(xPos, yPos);
    return sprite;
}

std::array<sf::Text, 3> getThreeTitlesText(const sf::Font& font, const std::vector<Game*>& games, const size_t index) {
    constexpr size_t MAX_TITLE_LENGTH = 100;
    std::array<sf::Text, 3> displayedTitles;
    for (size_t i = 0; i < displayedTitles.size(); ++i) {
        std::string title = games[index + i]->get_title();
        if (title.size() >= MAX_TITLE_LENGTH) {
            title = title.substr(0, 97) + "...";
        }
        for (size_t stringIndex = 24; stringIndex < title.size(); stringIndex += 25) {
            title.insert(stringIndex, "-\n");
        }
        displayedTitles[i].setString(title);
        displayedTitles[i].setFont(font);
        displayedTitles[i].setCharacterSize(25);
        displayedTitles[i].setFillColor(sf::Color::White);
        // Casting is fine, i is always less than 3
        displayedTitles[i].setPosition(50, 145.0F + 175.0F * static_cast<float>(i));
    }
    return displayedTitles;
}

std::array<sf::Text, 3> getThreeGenresText(const sf::Font& font, const std::vector<Game*>& games, const size_t index) {
    std::array<sf::Text, 3> displayedGenres;
    for (size_t i = 0; i < displayedGenres.size(); ++i) {
        auto genres = games[index + i]->get_genres();
        std::string genreString;
        for (size_t genreIndex = 0; (genreIndex < genres.size() && genreIndex <= 4); ++genreIndex) {
            if (genres[genreIndex].length() <= 20) {
                genreString += genres[genreIndex] + '\n';
                continue;
            }
            genreString += genres[genreIndex].substr(0, 17) + "...\n";
        }
        displayedGenres[i].setString(genreString);
        displayedGenres[i].setFont(font);
        displayedGenres[i].setCharacterSize(25);
        displayedGenres[i].setFillColor(sf::Color::White);
        // Casting is fine, i is always less than 3
        displayedGenres[i].setPosition(950.0F, 145.0F + 175.0F * static_cast<float>(i));
    }
    return displayedGenres;
}

std::array<sf::Text, 3> getThreePlatsText(const sf::Font& font, const std::vector<Game*>& games, const size_t index) {
    std::array<sf::Text, 3> displayedPlatforms;
    for (size_t i = 0; i < displayedPlatforms.size(); ++i) {
        if (std::string platformString = games[index + i]->get_platform(); platformString.size() <= 20) {
            displayedPlatforms[i].setString(platformString);
        } else {
            displayedPlatforms[i].setString(platformString.substr(0, 17) + "...");
        }
        displayedPlatforms[i].setFont(font);
        displayedPlatforms[i].setCharacterSize(25);
        displayedPlatforms[i].setFillColor(sf::Color::White);
        // Casting is fine, i is always less than 3
        displayedPlatforms[i].setPosition(650.0F, 145.0F + 175.0F * static_cast<float>(i));
    }
    return displayedPlatforms;
}

std::array<sf::Text, 3> getThreeRatingsText(const sf::Font& font, const std::vector<Game*>& games, const size_t indx) {
    std::array<sf::Text, 3> displayedRatings;
    for (size_t i = 0; i < displayedRatings.size(); ++i) {
        displayedRatings[i].setString(std::format("{:.2f}", games[indx + i]->get_score()) + " / 10");
        displayedRatings[i].setFont(font);
        displayedRatings[i].setCharacterSize(25);
        displayedRatings[i].setFillColor(sf::Color::White);
        // Casting is fine, i is always less than 3
        displayedRatings[i].setPosition(475.0F, 145.0F + 175.0F * static_cast<float>(i));
    }
    return displayedRatings;
}

std::array<sf::Text, 5> getSortTimeTexts(const sf::Font& font, const sf::RenderWindow& sortingWindow,
                                         std::vector<Game*>& games, bool (*comparator)(const Game*, const Game*)) {
    using std::chrono::duration_cast;
    using millis = std::chrono::milliseconds;
    using clock = std::chrono::high_resolution_clock;

    std::vector<Game*> mergeSortGames = games, binaryInsertionSortGames = games, stableSortGames = games;

    auto timeStart = clock::now();
    ts::timsort(games, comparator);
    puts("timsort done");
    const long long timsortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text timsortText;
    timsortText.setString("Timsort took " + std::to_string(timsortTime) + " milliseconds");
    std::cout << "Timsort took " + std::to_string(timsortTime) + " milliseconds";

    timeStart = clock::now();
    ms::mergeSort(mergeSortGames, comparator);
    puts("mergesort done");
    const long long mergeSortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text mergeSortText;
    mergeSortText.setString("Merge sort took " + std::to_string(mergeSortTime) + " milliseconds");

    timeStart = clock::now();
    ts::binaryInsertionSort(binaryInsertionSortGames, comparator);
    puts("BIS done");
    const long long binaryInsertionSortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text binaryInsertionSortText;
    binaryInsertionSortText.setString(
        "Binary insertion sort took " + std::to_string(binaryInsertionSortTime) + " milliseconds");

    timeStart = clock::now();
    std::ranges::stable_sort(stableSortGames.begin(), stableSortGames.end(), comparator);
    puts("stable_sort done");
    const long long stableSortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text stableSortText;
    stableSortText.setString("std::ranges::stable_sort took " + std::to_string(stableSortTime) + " milliseconds");

    // Deliberately avoiding the last element, it will be overwritten by the title anyway
    std::array<sf::Text, 5> sortTexts = {timsortText, mergeSortText, binaryInsertionSortText, stableSortText};
    // Set up the text attributes for the array
    for (size_t i = 0; i < sortTexts.size(); ++i) {
        sortTexts[i].setFont(font);
        sortTexts[i].setCharacterSize(30);
        sortTexts[i].setFillColor(sf::Color::White);
        const sf::FloatRect textRect = sortTexts[i].getLocalBounds();
        sortTexts[i].setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        sortTexts[i].setPosition(static_cast<float>(sortingWindow.getSize().x) / 2.0F,
                                 200.0F + 100.0F * static_cast<float>(i));
    }
    std::cout << "returning sortTexts\n";
    return sortTexts;
}

void renderSortingWindow(const sf::Font& font, const std::string& sortedField, std::vector<Game*>& games) {
    sf::RenderWindow sortingWindow(sf::VideoMode(900, 600), "GameSort", sf::Style::Close);
    sf::Color gatorBlue(0, 33, 165);
    sortingWindow.setMouseCursorVisible(true);
    sortingWindow.setKeyRepeatEnabled(true);
    sf::Text sortingText;
    sortingText.setString("Sorting by " + sortedField + "...");
    sortingText.setFont(font);
    sortingText.setCharacterSize(50);
    sortingText.setStyle(sf::Text::Bold | sf::Text::Italic);
    sortingText.setFillColor(sf::Color::White);

    const sf::FloatRect textRect = sortingText.getLocalBounds();
    sortingText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    sortingText.setPosition(static_cast<float>(sortingWindow.getSize().x) / 2.0f,
                            static_cast<float>(sortingWindow.getSize().y) / 2.0f);
    sortingWindow.clear(gatorBlue);
    sortingWindow.draw(sortingText);
    sortingWindow.display();

    // To keep the timing fair, pass a different vector to each sort
    // Timsort gets the original vector for no particular reason
    std::vector<Game*> mergeSortGames = games, binaryInsertionSortGames = games, stableSortGames = games;
    std::array<sf::Text, 5> sortingWindowTexts;
    if (sortedField == "title") {
        sortingWindowTexts = getSortTimeTexts(font, sortingWindow, games, Game::compareTitles);
    } else if (sortedField == "rating") {
        sortingWindowTexts = getSortTimeTexts(font, sortingWindow, games, Game::compareScores);
    } else if (sortedField == "genre") {
        sortingWindowTexts = getSortTimeTexts(font, sortingWindow, games, Game::compareGenres);
    } else {
        sortingWindowTexts = getSortTimeTexts(font, sortingWindow, games, Game::comparePlatform);
    }

    while (sortingWindow.isOpen()) {
        sf::Event event{};
        while (sortingWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                sortingWindow.close();
                return;
            }
            if (event.type == sf::Event::LostFocus) {
                sortingWindow.requestFocus();
            }
        }
        sortingWindow.clear(gatorBlue);
        for (const auto& text : sortingWindowTexts) {
            sortingWindow.draw(text);
        }
        sortingWindow.display();
    }
}

void renderMainWindow(const sf::Font& font, std::vector<Game*>& games) {
    // Shuffle the data to start to ensure a good spread
    std::random_device rd;
    std::mt19937 generator(rd());
    std::ranges::shuffle(games.begin(), games.end(), generator);

    // Index of games vector that is currently being displayed
    // ssize_t is okay, we only have 500,000ish games which can comfortably fit in just a long
    std::cout << "ssize t size: " << sizeof(ssize_t) << '\n';
    ssize_t gameIndex = 0;

    // Start up the main window
    sf::Color gatorOrange(250, 70, 22);
    sf::RenderWindow mainWindow(sf::VideoMode(1600, 700), "GameSort", sf::Style::Close);
    mainWindow.setMouseCursorVisible(true);
    mainWindow.setKeyRepeatEnabled(true);

    // Get headers
    Text welcomeText("Game Sort", font, 35, sf::Text::Underlined, sf::Text::Bold, sf::Color::White,
                     sf::Vector2f(250 / 2.0f, (380 / 2.0f) - 150));
    Text sortGamesText("Order by:", font, 28, sf::Text::Underlined, sf::Text::Bold, sf::Color::White,
                       sf::Vector2f(1400, (420 / 2.0f) - 150));

    // Point the singleton texture manager to the resource directory to get the textures and set the sprites
    TextureManager* textureManager = TextureManager::getInstance("../res");
    sf::Sprite nextArrow = getSprite(textureManager->getTexture("arrowNext"), 1455, 650, 0.25, 0.25);
    sf::Sprite prevArrow = getSprite(textureManager->getTexture("arrowPrevious"), 1355, 650, 0.25, 0.25);
    sf::Sprite title = getSprite(textureManager->getTexture("titleButton"), 1400, 145, 0.13, 0.13);
    sf::Sprite rating = getSprite(textureManager->getTexture("ratingButton"), 1400, 245, 0.13, 0.13);
    sf::Sprite genre = getSprite(textureManager->getTexture("genreButton"), 1400, 345, 0.13, 0.13);
    sf::Sprite platform = getSprite(textureManager->getTexture("platformButton"), 1400, 445, 0.13, 0.13);

    // Event-based loop
    while (mainWindow.isOpen()) {
        sf::Event event{};
        std::array<sf::Text, 3> displayedGenres = getThreeGenresText(font, games, gameIndex);
        std::array<sf::Text, 3> displayedPlatforms = getThreePlatsText(font, games, gameIndex);
        std::array<sf::Text, 3> displayedRatings = getThreeRatingsText(font, games, gameIndex);
        std::array<sf::Text, 3> displayedTitles = getThreeTitlesText(font, games, gameIndex);
        while (mainWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // Click X on the window
                games.clear();
                mainWindow.close();
                std::exit(0);
            }
            // If left mouse button pressed, check what was clicked
            if (event.type == sf::Event::MouseButtonPressed && mainWindow.hasFocus()) {
                sf::Vector2i mouse = sf::Mouse::getPosition(mainWindow);
                if (nextArrow.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("next arr press, goto next page");
                    if (gameIndex + 3 < static_cast<ssize_t>(games.size())) {
                        gameIndex += 3;
                    }
                }
                if (prevArrow.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    if (gameIndex - 3 >= 0) {
                        gameIndex -= 3;
                    }
                }
                std::string sortedField;
                if (title.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("title pressed, sort by title");
                    sortedField = "title";
                    renderSortingWindow(font, sortedField, games);
                }
                if (rating.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("rating pressed, sort by rating");
                    sortedField = "rating";
                    renderSortingWindow(font, sortedField, games);
                }
                if (genre.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("genre pressed, sort by genre");
                    sortedField = "genre";
                    renderSortingWindow(font, sortedField, games);
                }
                if (platform.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    puts("platform pressed, sort by platform");
                    sortedField = "platform";
                    renderSortingWindow(font, sortedField, games);
                }
            }
        }
        mainWindow.clear(gatorOrange);
        mainWindow.draw(title);
        mainWindow.draw(rating);
        mainWindow.draw(genre);
        mainWindow.draw(platform);
        mainWindow.draw(nextArrow);
        mainWindow.draw(prevArrow);
        mainWindow.draw(welcomeText.getText());
        mainWindow.draw(sortGamesText.getText());
        for (const auto& genreText : displayedGenres) {
            mainWindow.draw(genreText);
        }
        for (const auto& platformText : displayedPlatforms) {
            mainWindow.draw(platformText);
        }
        for (const auto& scoreText : displayedRatings) {
            mainWindow.draw(scoreText);
        }
        for (const auto& titleText : displayedTitles) {
            mainWindow.draw(titleText);
        }
        mainWindow.display();
        // Lock framerate to 60 to avoid high CPU consumption
        sf::sleep(sf::seconds(1.0F / 60.0F));
    }
}


sf::Text getLoadingWindowText(const sf::Font& font, const sf::RenderWindow& loadingWindow) {
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
    return text;
}

std::vector<Game*> loadGames(const sf::Font& font) {
    sf::RenderWindow loadingWindow(sf::VideoMode(900, 450), "GameSort", sf::Style::Close);
    loadingWindow.setMouseCursorVisible(true);
    sf::Text text = getLoadingWindowText(font, loadingWindow);
    loadingWindow.clear(sf::Color(0, 33, 165));
    loadingWindow.draw(text);
    loadingWindow.display();
    std::vector<Game*> games = parseJsons();
    loadingWindow.close();
    return games;
}

int main() {
    sf::Font font;
    if (!font.loadFromFile("../res/font.ttf")) {
        throw (std::runtime_error("unable to load font, aborting!"));
    }

    std::vector<Game*> games = loadGames(font);

    renderMainWindow(font, games);
    return 0;
}

// Get blacklisted characters from csv
std::vector<std::string> getBlacklist() {
    const char* path = "../config/blacklist.csv";
    std::ifstream file(path);
    if (!file.is_open()) {
        std::string msg = "Failed to find blacklist file at ";
        msg.append(path);
        throw std::ifstream::failure(msg);
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

// Ignore games that are possibly offensive
bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist) {
    std::string lowerTitle = game->get_title();
    std::ranges::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), tolower);
    for (const unsigned char c : lowerTitle) {
        // Remove games if they contain non-ascii or control characters
        if (c < 32 || c >= 127) {
            return true;
        }
    }
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
