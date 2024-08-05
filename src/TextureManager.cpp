#include "TextureManager.hpp"
#include <filesystem>

TextureManager* TextureManager::instance_ = nullptr;

TextureManager* TextureManager::getInstance(const std::string& directoryPath) {
    if (!instance_) {
        instance_ = new TextureManager(directoryPath);
    }
    return instance_;
}

TextureManager::TextureManager(const std::string& directoryPath) {
    loadTextures(directoryPath);
}

void TextureManager::loadTextures(const std::string& texturePath) {
    const std::filesystem::recursive_directory_iterator iter(texturePath);
    for (const auto& file : iter) {
        if (file.is_regular_file() && (file.path().extension() == ".png" || file.path().extension() == ".jpg")) {
            sf::Texture texture;
            if (!texture.loadFromFile(file.path().string())) {
                continue;
            }
            textures.emplace(file.path().filename().replace_extension().string(), texture);
        }
    }
    if (textures.empty()) {
        throw std::runtime_error("Textures missing, aborting!");
    }
}

sf::Texture& TextureManager::getTexture(const std::string& textureName) {
    return textures.at(textureName);
}

TextureManager::~TextureManager() {
    textures.clear();
    delete instance_;
    instance_ = nullptr;
}
