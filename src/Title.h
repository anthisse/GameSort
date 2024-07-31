#include <SFML/Graphics.hpp>
#pragma once

class Text {
public:

    Text(const std::string& content, const sf::Font& font, unsigned int characterSize, sf::Text::Style style, sf::Text::Style style2, const sf::Color& fillColor, const sf::Vector2f& position);
    sf::Text getText() const;

private:
    sf::Text text;
};