#include <cstdio>
#include <cstddef>
#include <cmath>
#include <limits>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

#include "Render.h"

void Renderer::drawRays(sf::RenderTarget &target, Player &player, const Map &map){
    try{
        const auto &grid = map.getGrid();
        const int rows = grid.size();
        const int cols = grid[0].size();
        auto player_pos = player.get_player_pose(); 
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        float angle = player_pos[2] * PI / 180.0f;
        float vTan = -tan(angle);
        float hTan = -1.0f / tan(angle);
        auto cellsize = map.getCellsize();
        size_t vdof = 0, hdof = 0;
        float hdist = std::numeric_limits<float>::max();
        float vdist = std::numeric_limits<float>::max();

        sf::Vector2f vrayPos, hrayPos, offset;
        if (cos(angle) > 0.001f){
            vrayPos.x = std::floor(player_pos_sf.x / cellsize) * cellsize + cellsize;
            offset.x = cellsize;
        } else if (cos(angle) < -0.001f){
            vrayPos.x = std::floor(player_pos_sf.x / cellsize) * cellsize - 0.01f;
            offset.x = -cellsize;
        } else {
            vdof = MaxRayCastingDepth;
        }
        vrayPos.y = (player_pos_sf.x - vrayPos.x) * vTan + player_pos_sf.y;
        offset.y = -offset.x * vTan;

        for(;vdof<MaxRayCastingDepth; vdof++){
            int mapX = (int)(vrayPos.x / cellsize);
            int mapY = (int)(vrayPos.y / cellsize);
            if (mapY < rows && mapX < cols && grid[mapY][mapX] && mapX >= 0 && mapY >= 0){
                auto det_x = vrayPos.x - player_pos_sf.x;
                auto det_y = vrayPos.y - player_pos_sf.y;
                vdist = std::hypotf(det_x, det_y);
                break;
            }
            vrayPos += offset;
        }

        if (sin(angle) < -0.001f){
            hrayPos.y = std::floor(player_pos_sf.y / cellsize) * cellsize - 0.01f;
            offset.y = -cellsize;
        } else if (sin(angle) > 0.001f){
            hrayPos.y = std::floor(player_pos_sf.y / cellsize) * cellsize + cellsize;
            offset.y = cellsize;
        } else {
            hdof = MaxRayCastingDepth;
        }
        hrayPos.x = (player_pos_sf.y - hrayPos.y) * hTan + player_pos_sf.x;
        offset.x = -offset.y * hTan;

        for(; hdof<MaxRayCastingDepth; hdof++){
            int mapX = (int)(hrayPos.x / cellsize);
            int mapY = (int)(hrayPos.y / cellsize);
            if (mapY < rows && mapX < cols && grid[mapY][mapX] && mapX >= 0 && mapY >= 0){
                auto det_x = hrayPos.x - player_pos_sf.x;
                auto det_y = hrayPos.y - player_pos_sf.y;
                hdist = std::hypotf(det_x, det_y);
                break;
            }
            hrayPos += offset;
        }
        sf::Vertex line[] = {
            {player_pos_sf},
            {hdist < vdist ? hrayPos : vrayPos}
        };
        target.draw(line, 2, sf::PrimitiveType::Lines);
    } catch (std::exception &e){
        printf("%s", e.what());
    }

}