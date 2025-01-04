#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
class Player {
    public:
    std::vector<float> get_player_pose();

    private:
    sf::Vector2f position;
    float angle;
};

#endif // !_PLAYER_H