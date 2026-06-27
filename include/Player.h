#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
class Player {
    public:
    std::vector<float> get_player_pose();
    void set_player_pose(sf::Vector2f pose);
    void draw(sf::RenderTarget &target);
    void set_player_size(float size);
    void update(float deltaTime);

    private:
    sf::Vector2f position;
    float player_size;
    float angle;
};

#endif // !_PLAYER_H