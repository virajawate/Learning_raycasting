#ifndef _RESOURCE_H
#define _RESOURCE_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

class Resources {
    public:
    static sf::Texture walltextures;
    static sf::Color wallTextureColor(int textureNo);
    static int wallTextureIndex(sf::Color color);
};

#endif // !_RESOURCE_H