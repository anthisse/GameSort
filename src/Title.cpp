#include "Title.h"
// referenced Elliot's minesweeper project

Text::Text(const std::string &content, const sf::Font &font, unsigned int characterSize, sf::Text::Style style, sf::Text::Style style2, const sf::Color &fillColor, const sf::Vector2f &position) {
    text.setString(content);
    text.setFont(font);
    text.setCharacterSize(characterSize);
    text.setStyle(style | style2);
    text.setFillColor(fillColor);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(position);
}

sf::Text Text::getText() const {
    return text;
}