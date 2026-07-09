#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include "Player.h"
#include "Map.h"

constexpr unsigned int ScreenW = 1280;
constexpr unsigned int ScreenH = 720;
constexpr unsigned int NUM_RAYS = 240;
constexpr float COLUMN_WIDTH = ScreenW / (float)NUM_RAYS;

struct Ray {
    sf::Vector2f hitPosition;
    float distance;
    bool hit;
};

class Renderer{
public:
    void draw3dview(sf::RenderTarget &target, Player &player, const Map &map);
    void drawRays(sf::RenderTarget &target, Player &player, const Map &map);
    Ray castRay(sf::Vector2f start, float angleInDegrees, const Map &map, bool fps_mode);

private:
    const float PI = 3.1427;
    const size_t MaxRayCastingDepth = 32;
    const size_t player_fov = 60;
};

#endif // !_RENDERER_H