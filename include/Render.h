#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include "Player.h"

class Renderer{
public:
    void drawRays(sf::RenderTarget &target, Player &player, float cellSize);

private:
    float PI = 3.1427;
};

#endif // !_RENDERER_H