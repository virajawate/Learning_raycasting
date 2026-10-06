#include "Resources.h"
#include <SFML/Graphics/Texture.hpp>

sf::Texture Resources::walltextures{};

sf::Color Resources::wallTextureColor(int textureNo) {
    switch (textureNo) {
        case 0: return sf::Color::White;
        case 1: return sf::Color::Cyan;
        case 2: return sf::Color::Red;
        case 3: return sf::Color::Green;
        case 4: return sf::Color::Yellow;
        case 5: return sf::Color(255, 26, 0, 255);
        case 6: return sf::Color(128, 64, 0, 255);
        case 7: return sf::Color(64, 128, 0, 255);
        case 8: return sf::Color(255, 128, 0, 255);
        case 9: return sf::Color(128, 0, 255, 255);
        case 10: return sf::Color(0, 128, 255, 255);
        default: return sf::Color::Black;
    }
}

int Resources::wallTextureIndex(sf::Color color) {
    for (int textureNo = 0; textureNo <= 10; ++textureNo) {
        if (wallTextureColor(textureNo) == color) {
            return textureNo;
        }
    }
    return -1;
}