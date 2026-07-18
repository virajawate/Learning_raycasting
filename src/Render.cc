#include <cstdio>
#include <cstddef>
#include <cmath>
#include <limits>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>

#include "Render.h"

Ray Renderer::castRay(sf::Vector2f start, float angleInDegrees, const Map &map, bool fps_mode = false){
    const auto &grid = map.getGridColor();
    auto cellsize = map.getCellsize();
    const int rows = grid.size();
    const int cols = grid[0].size();
    
    float angle = angleInDegrees * PI / 180.0f;
    float vTan = -tan(angle);
    float hTan = -1.0f / tan(angle);
    size_t vdof = 0, hdof = 0;
    bool hit = false;
    float hdist = std::numeric_limits<float>::max();
    float vdist = std::numeric_limits<float>::max();

    sf::Vector2u vMapPos, hMapPos;
    sf::Vector2f vRayPos, hRayPos, offset;
    if (cos(angle) > 0.001f){
        vRayPos.x = std::floor(start.x / cellsize) * cellsize + cellsize;
        offset.x = cellsize;
    } else if (cos(angle) < -0.001f){
        vRayPos.x = std::floor(start.x / cellsize) * cellsize - 0.01f;
        offset.x = -cellsize;
    } else {
        vdof = MaxRayCastingDepth;
    }
    vRayPos.y = (start.x - vRayPos.x) * vTan + start.y;
    offset.y = -(offset.x * vTan);
    unsigned int mapX = ( unsigned int )(vRayPos.x / cellsize);
    unsigned int mapY = ( unsigned int )(vRayPos.y / cellsize);

    for(;vdof<MaxRayCastingDepth; vdof++){
        mapX = (unsigned int)(vRayPos.x / cellsize);
        mapY = (unsigned int)(vRayPos.y / cellsize);
        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black){
            hit = true;
            auto det_x = vRayPos.x - start.x;
            auto det_y = vRayPos.y - start.y;
            vdist = std::hypotf(det_x, det_y);
            vMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        vRayPos += offset;
    }

    if (sin(angle) < -0.001f){
        hRayPos.y = std::floor(start.y / cellsize) * cellsize - 0.01f;
        offset.y = -cellsize;
    } else if (sin(angle) > 0.001f){
        hRayPos.y = std::floor(start.y / cellsize) * cellsize + cellsize;
        offset.y = cellsize;
    } else {
        hdof = MaxRayCastingDepth;
    }
    hRayPos.x = (start.y - hRayPos.y) * hTan + start.x;
    offset.x = -offset.y * hTan;
    mapX = (unsigned int)(hRayPos.x / cellsize);
    mapY = (unsigned int)(hRayPos.y / cellsize);
    
    for(; hdof<MaxRayCastingDepth; hdof++){
        mapX = (unsigned int)(hRayPos.x / cellsize);
        mapY = (unsigned int)(hRayPos.y / cellsize);

        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black){
            hit = true;
            auto det_x = hRayPos.x - start.x;
            auto det_y = hRayPos.y - start.y;
            hdist = std::hypotf(det_x, det_y);
            hMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        hRayPos += offset;
    }
    return Ray{(hdist < vdist ? hRayPos : vRayPos), (hdist < vdist ? hMapPos : vMapPos),  std::min(hdist, vdist), hit};
}

void Renderer::draw3dview(sf::RenderTarget &target, Player &player, const Map &map){
    try{
        sf::RectangleShape rectangle(sf::Vector2f(ScreenW, ScreenH / 2.0f));
        rectangle.setFillColor(sf::Color(100, 170, 250));
        target.draw(rectangle);

        rectangle.setPosition({0.0f, ScreenH / 2.0f});
        rectangle.setFillColor(sf::Color(70, 70, 70));
        target.draw(rectangle);

        auto player_pos = player.get_player_pose();
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        float angle = player_pos[2] - player_fov/2.0f;
        float angleIncrement = player_fov / (float)NUM_RAYS;
        const float maxRenderDistance = MaxRayCastingDepth * map.getCellsize();
        const float maxFogDistance = maxRenderDistance / 1.5f;
        for(size_t i =0; i < NUM_RAYS; i++, angle += angleIncrement){
            Ray ray = castRay(player_pos_sf, angle, map, true);
            if (ray.hit){
                ray.distance *= std::cos((player_pos[2] - angle) * PI / 180.0f);
                float wallHeight = (map.getCellsize() * ScreenH) / ray.distance;
                if(wallHeight > ScreenH){
                    wallHeight = ScreenH;
                }
                float brightness = 1.0f - (ray.distance / maxRenderDistance);
                if(brightness < 0.0f){
                    brightness = 0.0f;
                }
                float shade = (ray.isHitVertical ? 0.8f : 1.0f) * brightness;
                float walloffset = ScreenH / 2.0f - wallHeight / 2.0f;
                sf::RectangleShape column(sf::Vector2f(COLUMN_WIDTH, wallHeight));
                column.setPosition({i*COLUMN_WIDTH, walloffset});
                fogAlpha = 3.5 * (ray.distance / maxRenderDistance);
                sf::Color color = map.getGridColor()[ray.mapPosition.y][ray.mapPosition.x];
                color = sf::Color(color.r * shade, color.g * shade, color.b * shade);
                column.setFillColor(
                    sf::Color(
                        ((1.0f - fogAlpha) * color.r + fogAlpha * fogColor.r),
                        ((1.0f - fogAlpha) * color.b + fogAlpha * fogColor.b),
                        ((1.0f - fogAlpha) * color.g + fogAlpha * fogColor.g)
                    )
                );
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
        for(float angle = player_pos[2] - player_fov/2.0f; angle < player_pos[2] + player_fov/2.0; angle += 1){
            Ray ray = castRay(player_pos_sf, angle, map, false);
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