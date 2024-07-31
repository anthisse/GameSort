#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
using namespace std;

class TextureManager {
    static unordered_map<string, sf::Texture> textures;

public:
    static sf::Texture& getTexture(string textureName);
};
