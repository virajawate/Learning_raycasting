#include "Render.h"
#include <SFML/System/Vector2.hpp>

void Renderer::drawRays(sf::RenderTarget &target, Player &player, float cellSize){
    auto player_pos = player.get_player_pose(); 
    float angle = player_pos[2] * PI / 180.0f;
    float aTan = -1.0/ tan(angle);

    sf::Vector2f rayPos, offset;
    if (angle > PI){
        rayPos.y = std::round(player_pos[1] / cellSize) * cellSize;
        rayPos.x = (player_pos[1] - rayPos.y) * aTan + player_pos[0];
        offset.y = -cellSize;
        offset.x = -offset.y * aTan;
    } else if (angle < PI){
        rayPos.y = std::round(player_pos[1] / cellSize) * cellSize + cellSize;
        rayPos.x = (player_pos[1] - rayPos.y) * aTan + player_pos[0];
        offset.y = cellSize;
        offset.x = -offset.y * aTan;
    } else {
        return;
    }
}