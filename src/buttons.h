#include <string>
#include "TextureManager.h"

struct Buttons {
    Buttons(const std::string& textureKey, float positionX, float positionY);
    sf::Sprite getSprite();

//private:
    sf::Texture* texture;
    sf::Sprite sprite;
};