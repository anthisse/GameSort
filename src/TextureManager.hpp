#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class TextureManager {
public:
    // Singleton, delete copy constructor and copy assignment operator
    TextureManager(TextureManager& rhs) = delete;

    TextureManager(TextureManager&& rhs) = delete;


    void operator=(const TextureManager& rhs) = delete;

    void operator=(const TextureManager&& rhs) = delete;

    sf::Texture& getTexture(const std::string& textureName);

    static TextureManager* getInstance(const std::string& directoryPath);


    ~TextureManager();

private:
    explicit TextureManager(const std::string& directoryPath);

    static TextureManager* instance;

    void loadTextures(const std::string& texturePath);

    std::unordered_map<std::string, sf::Texture> textures;
};
