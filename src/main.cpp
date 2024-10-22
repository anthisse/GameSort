#include <algorithm>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>

// SFML graphics library
#include <SFML/Graphics.hpp>

// Objects that we wish to sort
#include "Game.hpp"

// Parse json files. Provided by https://github.com/simdjson/simdjson
#include "../lib/simdjson.h"
#include "timsort.hpp"
#include "mergesort.hpp"
#include "TextureManager.hpp"

std::vector<Game*> renderLoadingWindow(const sf::Font& font);

sf::Text getLoadingWindowText(const sf::Font& font, const sf::RenderWindow& loadingWindow);

std::vector<Game*> parseJsons();

std::vector<std::string> getBlacklist();

std::vector<std::string> getGenres(simdjson::simdjson_result<simdjson::ondemand::value> json);

bool isBlacklisted(const Game* game, const std::vector<std::string>& blacklist);

void renderMainWindow(const sf::Font& font, std::vector<Game*>& games);

sf::Sprite getSprite(const sf::Texture& texture, float xPos, float yPos, float xScale, float yScale);

std::array<sf::Text, 3> getThreeTitlesText(const sf::Font& font, const std::vector<Game*>& games, size_t index);

std::array<sf::Text, 3> getThreeRatingsText(const sf::Font& font, const std::vector<Game*>& games, size_t index);

std::array<sf::Text, 3> getThreeGenresText(const sf::Font& font, const std::vector<Game*>& games, size_t index);

std::array<sf::Text, 3> getThreePlatsText(const sf::Font& font, const std::vector<Game*>& games, size_t index);

void renderSortingWindow(const sf::Font& font, const std::string& sortedField, std::vector<Game*>& games);

std::array<sf::Text, 5> getSortTimeTexts(const sf::Font& font, const sf::RenderWindow& sortingWindow,
                                         std::vector<Game*>& games, bool (*comparator)(const Game*, const Game*));

int main() {
    sf::Font font;
    if (!font.loadFromFile("../res/font.ttf")) {
        throw (std::runtime_error("unable to load font, aborting!"));
    }

    std::vector<Game*> games = renderLoadingWindow(font);

    renderMainWindow(font, games);
    return 0;
}

std::vector<Game*> renderLoadingWindow(const sf::Font& font) {
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

sf::Text getLoadingWindowText(const sf::Font& font, const sf::RenderWindow& loadingWindow) {
    sf::Text text;
    text.setString("Parsing data...");
    text.setFont(font);
    text.setCharacterSize(50);
    text.setStyle(sf::Text::Bold | sf::Text::Italic);
    text.setFillColor(sf::Color::White);

    const sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0F, textRect.top + textRect.height / 2.0F);
    text.setPosition(static_cast<float>(loadingWindow.getSize().x) / 2.0F,
                     static_cast<float>(loadingWindow.getSize().y) / 2.0F);
    return text;
}

std::vector<Game*> parseJsons() {
    std::vector<std::string> blacklist;
    try {
        blacklist = getBlacklist();
    } catch (std::ifstream::failure& e) {
        std::cerr << e.what() << "\nblacklist not functional, config/blacklist.csv not found.\n";
    }
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
    printf("number of games: %llu\n", games.size());
    return games;
}

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

void renderMainWindow(const sf::Font& font, std::vector<Game*>& games) {
    // Shuffle the data to ensure a good spread to start
    std::random_device rd;
    std::mt19937 generator(rd());
    std::ranges::shuffle(games.begin(), games.end(), generator);

    // Index of games vector that is currently being displayed
    // ssize_t is okay, we only have 500,000ish games which can comfortably fit in a long
    ssize_t gameIndex = 0;

    // Start up the main window
    sf::Color gatorOrange(250, 70, 22);
    sf::RenderWindow mainWindow(sf::VideoMode(1600, 700), "GameSort", sf::Style::Close);
    mainWindow.setMouseCursorVisible(true);
    mainWindow.setKeyRepeatEnabled(true);

    // Get headers
    sf::Text welcomeText, sortGamesText;

    welcomeText.setString("Game Sort");
    welcomeText.setFont(font);
    welcomeText.setCharacterSize(35);
    welcomeText.setStyle(sf::Text::Underlined | sf::Text::Bold);
    welcomeText.setFillColor(sf::Color::White);
    sf::FloatRect welcomeTextRect = welcomeText.getLocalBounds();
    welcomeText.setOrigin(welcomeTextRect.left + welcomeTextRect.width / 2.0F,
                          welcomeTextRect.top + welcomeTextRect.height / 2.0F);
    welcomeText.setPosition(250.0F / 2.0F, 380.0F / 2.0F - 150.0F);

    sortGamesText.setString("Order by:");
    sortGamesText.setFont(font);
    sortGamesText.setCharacterSize(28);
    sortGamesText.setStyle(sf::Text::Underlined | sf::Text::Bold);
    sortGamesText.setFillColor(sf::Color::White);
    sf::FloatRect sortGamesTextRect = sortGamesText.getLocalBounds();
    sortGamesText.setOrigin(sortGamesTextRect.left + sortGamesTextRect.width / 2.0F,
                            sortGamesTextRect.top + sortGamesTextRect.height / 2.0F);
    sortGamesText.setPosition(1400.0F, 420.0F / 2.0F - 150.0F);


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
                    sortedField = "title";
                    renderSortingWindow(font, sortedField, games);
                }
                if (rating.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    sortedField = "rating";
                    renderSortingWindow(font, sortedField, games);
                }
                if (genre.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
                    sortedField = "genre";
                    renderSortingWindow(font, sortedField, games);
                }
                if (platform.getGlobalBounds().contains(mainWindow.mapPixelToCoords(mouse))) {
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
        mainWindow.draw(welcomeText);
        mainWindow.draw(sortGamesText);
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


sf::Sprite getSprite(const sf::Texture& texture, const float xPos, float const yPos,
                     float const xScale, float const yScale) {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    // Scale the sprite so that it fits nicely on the screen and set its position
    sprite.setScale(xScale, yScale);
    sprite.setOrigin(static_cast<float>(texture.getSize().x) / 2.0F, static_cast<float>(texture.getSize().y) / 2.0F);
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

std::array<sf::Text, 3> getThreeRatingsText(const sf::Font& font, const std::vector<Game*>& games, const size_t index) {
    std::array<sf::Text, 3> displayedRatings;
    for (size_t i = 0; i < displayedRatings.size(); ++i) {
        displayedRatings[i].setString(std::format("{:.2f}", games[index + i]->get_score()) + " / 10");
        displayedRatings[i].setFont(font);
        displayedRatings[i].setCharacterSize(25);
        displayedRatings[i].setFillColor(sf::Color::White);
        // Casting is fine, i is always less than 3
        displayedRatings[i].setPosition(475.0F, 145.0F + 175.0F * static_cast<float>(i));
    }
    return displayedRatings;
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

std::array<sf::Text, 5> getSortTimeTexts(const sf::Font& font, const sf::RenderWindow& sortingWindow,
                                         std::vector<Game*>& games, bool (*comparator)(const Game*, const Game*)) {
    using std::chrono::duration_cast;
    using millis = std::chrono::milliseconds;
    using clock = std::chrono::high_resolution_clock;

    std::vector<Game*> mergeSortGames = games, binaryInsertionSortGames = games, stableSortGames = games;

    // TIM SORT 
    auto timeStart = clock::now();
    ts::timsort(games, comparator);
    const long long timsortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text timsortText;
    timsortText.setString("Timsort took " + std::to_string(timsortTime) + " milliseconds");

    // MERGE SORT 
    timeStart = clock::now();
    ms::mergeSort(mergeSortGames, comparator);
    const long long mergeSortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text mergeSortText;
    mergeSortText.setString("Merge sort took " + std::to_string(mergeSortTime) + " milliseconds");

    // BINARY INSERTION SORT
    timeStart = clock::now();
    ts::binaryInsertionSort(binaryInsertionSortGames, comparator);
    const long long binaryInsertionSortTime = (duration_cast<millis>(clock::now() - timeStart)).count();
    sf::Text binaryInsertionSortText;
    binaryInsertionSortText.setString(
        "Binary insertion sort took " + std::to_string(binaryInsertionSortTime) + " milliseconds");

    // STABLE_SORT
    timeStart = clock::now();
    std::ranges::stable_sort(stableSortGames.begin(), stableSortGames.end(), comparator);
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
        sortTexts[i].setOrigin(textRect.left + textRect.width / 2.0F, textRect.top + textRect.height / 2.0F);
        sortTexts[i].setPosition(static_cast<float>(sortingWindow.getSize().x) / 2.0F,
                                 200.0F + 100.0F * static_cast<float>(i));
    }
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

    const sf::FloatRect sortingTextRect = sortingText.getLocalBounds();
    sortingText.setOrigin(sortingTextRect.left + sortingTextRect.width / 2.0F,
                          sortingTextRect.top + sortingTextRect.height / 2.0F);
    sortingText.setPosition(static_cast<float>(sortingWindow.getSize().x) / 2.0F,
                            static_cast<float>(sortingWindow.getSize().y) / 2.0F);
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

    sf::Text headerText;
    headerText.setString("Sort completed! Stats:");
    headerText.setFont(font);
    headerText.setCharacterSize(50);
    headerText.setStyle(sf::Text::Bold);
    headerText.setFillColor(sf::Color::White);
    const sf::FloatRect headerTextRect = headerText.getLocalBounds();
    headerText.setOrigin(headerTextRect.left + headerTextRect.width / 2.0F,
                         headerTextRect.top + headerTextRect.height / 2.0F);
    headerText.setPosition(static_cast<float>(sortingWindow.getSize().x) / 2.0F, 100.0F);
    sortingWindowTexts[4] = headerText;

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
        sf::sleep(sf::seconds(1.0F / 60.0F));
    }
}
