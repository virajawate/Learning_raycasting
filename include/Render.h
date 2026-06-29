#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include "Player.h"
#include "Map.h"

struct Ray {
    sf::Vector2f hitPosition;
    float distance;
    bool hit;
};

class Renderer{
public:
    void drawRays(sf::RenderTarget &target, Player &player, const Map &map);
    Ray castRay(sf::Vector2f start, float angleInDegrees, const Map &map);

private:
    const float PI = 3.1427;
    const size_t MaxRayCastingDepth = 32;
};

#endif // !_RENDERER_H