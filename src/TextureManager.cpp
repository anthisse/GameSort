#include "TextureManager.h"
#include <iostream> // TODO remove me
#include <filesystem>

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::getInstance(const std::string& directoryPath) {
    if (!instance) {
        instance = new TextureManager(directoryPath);
    }
    return instance;
}

TextureManager::TextureManager(const std::string& directoryPath) {
    loadTextures(directoryPath);
}

void TextureManager::loadTextures(const std::string& texturePath) {
    const std::filesystem::recursive_directory_iterator iter(texturePath);
    for (const auto& file : iter) {
        if (file.is_regular_file() && (file.path().extension() == ".png" || file.path().extension() == ".jpg")) {
            std::cout << "Found image/texture " << file.path().filename() << "\n";
            sf::Texture texture;
            if (!texture.loadFromFile(file.path().string())) {
                std::cerr << "Loading image" << file.path() << " failed\n";
                continue;
            }
            textures.emplace(file.path().filename().replace_extension().string(), texture);
        }
    }
    if (textures.empty()) {
        throw std::runtime_error("Textures missing, aborting!");
    }
}

sf::Texture& TextureManager::operator[](const std::string& textureName) {
    return getTexture(textureName);
}

sf::Texture& TextureManager::operator[](const char textureName[]) {
    return getTexture(textureName);
}

sf::Texture& TextureManager::getTexture(const std::string& textureName) {
    return textures.at(textureName);
}

TextureManager::~TextureManager() {
    textures.clear();
}

