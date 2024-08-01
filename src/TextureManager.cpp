#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "TextureManager.h"
// code referenced by Elliot's minesweeper project

unordered_map<string, sf::Texture> TextureManager::textures;

sf::Texture& TextureManager::getTexture(std::string textureName) {
    auto result = textures.find(textureName);
    if(result == textures.end()) {                                          // Texture does not already exist in the map, go get it
        sf::Texture newTexture;
        newTexture.loadFromFile("../res/images/" + textureName + ".png");
        textures[textureName] = newTexture;
        return textures[textureName];
    }
    else {
        return result->second;                   // Texture already exists, return it
    }

}
