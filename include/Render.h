#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include "Player.h"
#include "Map.h"

class Renderer{
public:
    void drawRays(sf::RenderTarget &target, Player &player,const Map &map);

private:
    const float PI = 3.1427;
    const size_t MaxRayCastingDepth = 32;
};

#endif // !_RENDERER_H