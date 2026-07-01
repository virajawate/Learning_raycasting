#include <cstdio>
#include <cstddef>
#include <cmath>
#include <limits>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

#include "Render.h"

Ray Renderer::castRay(sf::Vector2f start, float angleInDegrees, const Map &map){
    
    const auto &grid = map.getGrid();
    auto cellsize = map.getCellsize();
    const int rows = grid.size();
    const int cols = grid[0].size();
    
    float angle = angleInDegrees * PI / 180.0f;
    float vTan = -tan(angle);
    float hTan = -1.0f / tan(angle);
    size_t vdof = 0, hdof = 0;
    bool vHit = false, hHit = false;
    float hdist = std::numeric_limits<float>::max();
    float vdist = std::numeric_limits<float>::max();
    


    sf::Vector2f vrayPos, hrayPos, offset;
    if (cos(angle) > 0.001f){
        vrayPos.x = std::floor(start.x / cellsize) * cellsize + cellsize;
        offset.x = cellsize;
    } else if (cos(angle) < -0.001f){
        vrayPos.x = std::floor(start.x / cellsize) * cellsize - 0.01f;
        offset.x = -cellsize;
    } else {
        vdof = MaxRayCastingDepth;
    }
    vrayPos.y = (start.x - vrayPos.x) * vTan + start.y;
    offset.y = -offset.x * vTan;

    for(;vdof<MaxRayCastingDepth; vdof++){
        int mapX = (int)(vrayPos.x / cellsize);
        int mapY = (int)(vrayPos.y / cellsize);
        if (mapY < rows && mapX < cols && grid[mapY][mapX] && mapX >= 0 && mapY >= 0){
            vHit = true;
            auto det_x = vrayPos.x - start.x;
            auto det_y = vrayPos.y - start.y;
            vdist = std::hypotf(det_x, det_y);
            break;
        }
        vrayPos += offset;
    }

    if (sin(angle) < -0.001f){
        hrayPos.y = std::floor(start.y / cellsize) * cellsize - 0.01f;
        offset.y = -cellsize;
    } else if (sin(angle) > 0.001f){
        hrayPos.y = std::floor(start.y / cellsize) * cellsize + cellsize;
        offset.y = cellsize;
    } else {
        hdof = MaxRayCastingDepth;
    }
    hrayPos.x = (start.y - hrayPos.y) * hTan + start.x;
    offset.x = -offset.y * hTan;

    for(; hdof<MaxRayCastingDepth; hdof++){
        int mapX = (int)(hrayPos.x / cellsize);
        int mapY = (int)(hrayPos.y / cellsize);
        if (mapY < rows && mapX < cols && grid[mapY][mapX] && mapX >= 0 && mapY >= 0){
            hHit = true;
            auto det_x = hrayPos.x - start.x;
            auto det_y = hrayPos.y - start.y;
            hdist = std::hypotf(det_x, det_y);
            break;
        }
        hrayPos += offset;
    }
    return Ray{(hdist < vdist ? hrayPos : vrayPos), std::min(hdist, vdist), vHit || hHit};
}

void Renderer::draw3dview(sf::RenderTarget &target, Player &player, const Map &map){
    try{
        auto player_pos = player.get_player_pose();
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        float angle = player_pos[2] - player_fov/2.0f;
        float angleIncrement = player_fov / (float)NUM_RAYS;
        for(size_t i =0; i < NUM_RAYS; i++, angle += angleIncrement){
            Ray ray = castRay(player_pos_sf, angle, map);
            if (ray.hit){
                ray.distance *= std::cos((player_pos[2] - angle) * PI / 180.0f);
                float wallHeight = (map.getCellsize() * ScreenH) / ray.distance;
                if(wallHeight > ScreenH){
                    wallHeight = ScreenH;
                }
                float walloffset = ScreenH / 2.0f - wallHeight / 2.0f;
                sf::RectangleShape column(sf::Vector2f(COLUMN_WIDTH, wallHeight));
                column.setPosition({i*COLUMN_WIDTH, walloffset});
                target.draw(column);
            }
        }
        
    } catch (std::exception &e){
        printf("%s", e.what());
    }
}

void Renderer::drawRays(sf::RenderTarget &target, Player &player, const Map &map){
    try{
        auto player_pos = player.get_player_pose();
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        for(float angle = player_pos[2] - player_fov/2.0f; angle < player_pos[2] + player_fov; angle += 1){
            Ray ray = castRay(player_pos_sf, angle, map);
            if (ray.hit){
                sf::Vertex line[] = {
                    {player_pos_sf},
                    {ray.hitPosition}
                };
                target.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
        
    } catch (std::exception &e){
        printf("%s", e.what());
    }

}