#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include "Map.h"
constexpr float PLAYER_TURN_SPEED = 100.0f;

class Player {
    public:
    void set_player_pose(sf::Vector2f pose);
    void update(float deltaTime, Map &map);
    std::vector<float> get_player_pose();
    void draw(sf::RenderTarget &target);
    void set_player_size(float size);
    void reset();

    private:
    sf::Vector2f position;
    float player_size;
    float angle;
    float move_x, move_y;
};

#endif // !_PLAYER_H