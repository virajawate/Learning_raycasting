#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Player.h"
#include "Map.h"

constexpr unsigned int ScreenW = 1280;
constexpr unsigned int ScreenH = 720;
constexpr unsigned int NUM_RAYS = 800;
constexpr float COLUMN_WIDTH = ScreenW / (float)NUM_RAYS;

struct Ray {
    sf::Vector2f hitPosition;
    sf::Vector2u mapPosition;
    float distance;
    bool hit;
    bool isHitVertical;
};

class Renderer{
public:
    void draw3dview(sf::RenderTarget &target, Player &player, const Map &map);
    void drawRays(sf::RenderTarget &target, Player &player, const Map &map);
    Ray castRay(sf::Vector2f start, float angleInDegrees, const Map &map, bool fps_mode);
    void init();
    Renderer();

private:
    float fogAlpha = 0.6;
    const float PI = 3.1427;
    const size_t player_fov = 60;
    const size_t MaxRayCastingDepth = 128;
    sf::Color fogColor = sf::Color(0, 0, 0);

    sf::Texture wall_texture;
    sf::Sprite wall_sprite;
    std::string texture_file = "texture/textures.png";
    
};

#endif // !_RENDERER_H