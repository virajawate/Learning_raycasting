#include <cmath>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

#include "Render.h"

void Renderer::drawRays(sf::RenderTarget &target, Player &player, Map map){
    auto player_pos = player.get_player_pose(); 
    float angle = player_pos[2] * PI / 180.0f;
    float aTan = -1.0f / tan(angle);
    auto cellsize = map.getCellsize();

    sf::Vector2f rayPos, offset;
    if (sin(angle) < -0.001f){
        rayPos.y = std::round(player_pos[1] / cellsize) * cellsize;
        offset.y = -cellsize;
    } else if (sin(angle) > 0.001f){
        rayPos.y = std::round(player_pos[1] / cellsize) * cellsize + cellsize;
        offset.y = cellsize;
    } else {
        return;
    }
    rayPos.x = (player_pos[1] - rayPos.y) * aTan + player_pos[0];
    offset.x = -offset.y * aTan;

    const auto &grid = map.getGrid();
    
    sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
    for(size_t i=0; i<MaxRayCastingDepth; i++){
        int mapX = (int)(rayPos.x / cellsize);
        int mapY = (int)(rayPos.y / cellsize);

        if (mapY < static_cast<int>(grid.size()) &&
            mapX < static_cast<int>(grid[mapY].size()) &&
            grid[mapY][mapX])
        {
            break;
        }
        rayPos += offset;

        sf::Vertex line[] = {
            {player_pos_sf},
            {rayPos}
        };
        target.draw(line, 2, sf::PrimitiveType::Lines);
    }
}