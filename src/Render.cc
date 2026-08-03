#include "Render.h"

Renderer::Renderer() : wall_texture(), wall_sprite(wall_texture) {}

void Renderer::init()
{
    if (!wall_texture.loadFromFile(wall_texture_file) && !floor_texture.loadFromFile(floor_texture_file))
    {
        std::cerr << "Texture file NOT LOADED." << wall_texture_file << std::endl;
        return;
    }
    else
    {
        std::cout << "Texture File Loaded" << std::endl;
    }

    if (wall_texture.getSize().x != wall_texture.getSize().y)
    {
        std::cerr << "ERROR : Wall Texture is not square" << std::endl;
        return;
    }
    if (floor_texture.getSize().x != floor_texture.getSize().y)
    {
        std::cerr << "ERROR : Floor Texture is not square" << std::endl;
        return;
    }
    wall_sprite = sf::Sprite(wall_texture);
}

Ray Renderer::castRay(sf::Vector2f start, float angleInDegrees, const Map &map, bool fps_mode = false)
{
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
    if (cos(angle) > 0.001f)
    {
        vRayPos.x = std::floor(start.x / cellsize) * cellsize + cellsize;
        offset.x = cellsize;
    }
    else if (cos(angle) < -0.001f)
    {
        vRayPos.x = std::floor(start.x / cellsize) * cellsize - 0.01f;
        offset.x = -cellsize;
    }
    else
    {
        vdof = MaxRayCastingDepth;
    }
    vRayPos.y = (start.x - vRayPos.x) * vTan + start.y;
    offset.y = -(offset.x * vTan);
    unsigned int mapX = (unsigned int)(vRayPos.x / cellsize);
    unsigned int mapY = (unsigned int)(vRayPos.y / cellsize);

    for (; vdof < MaxRayCastingDepth; vdof++)
    {
        mapX = (unsigned int)(vRayPos.x / cellsize);
        mapY = (unsigned int)(vRayPos.y / cellsize);
        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            auto det_x = vRayPos.x - start.x;
            auto det_y = vRayPos.y - start.y;
            vdist = std::hypotf(det_x, det_y);
            vMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        vRayPos += offset;
    }

    if (sin(angle) < -0.001f)
    {
        hRayPos.y = std::floor(start.y / cellsize) * cellsize - 0.01f;
        offset.y = -cellsize;
    }
    else if (sin(angle) > 0.001f)
    {
        hRayPos.y = std::floor(start.y / cellsize) * cellsize + cellsize;
        offset.y = cellsize;
    }
    else
    {
        hdof = MaxRayCastingDepth;
    }
    hRayPos.x = (start.y - hRayPos.y) * hTan + start.x;
    offset.x = -offset.y * hTan;
    mapX = (unsigned int)std::floor(vRayPos.x / cellsize);
    mapY = (unsigned int)std::floor(hRayPos.y / cellsize);

    for (; hdof < MaxRayCastingDepth; hdof++)
    {
        mapX = (unsigned int)(hRayPos.x / cellsize);
        mapY = (unsigned int)(hRayPos.y / cellsize);

        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            auto det_x = hRayPos.x - start.x;
            auto det_y = hRayPos.y - start.y;
            hdist = std::hypotf(det_x, det_y);
            hMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        hRayPos += offset;
    }
    bool vertical = (vdist < hdist);
    return Ray{(vertical ? vRayPos : hRayPos), (vertical ? hMapPos : vMapPos), std::min(vdist, hdist), hit, vertical};
}

void Renderer::draw3dview(sf::RenderTarget &target, Player &player, const Map &map)
{
    try
    {
        sf::RectangleShape rectangle(sf::Vector2f(ScreenW, ScreenH / 2.0f));
        rectangle.setFillColor(sf::Color(100, 170, 250));
        target.draw(rectangle);
        rectangle.setPosition({0.0f, ScreenH / 2.0f});
        rectangle.setFillColor(sf::Color(70, 70, 70));
        target.draw(rectangle);
        auto player_pos = player.get_player_pose();
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        const sf::Color fogColor = sf::Color(100, 170, 250);
        const float maxRenderDistance = MaxRayCastingDepth * map.getCellsize();
        const float maxFogDistance = maxRenderDistance / 4.0f;
        sf::RectangleShape column({1.0f, 1.0f});
        float angle = player_pos[2] - player_fov / 2.0f;
        float angleIncrement = player_fov / (float)NUM_RAYS;
        for (size_t i = 0; i < NUM_RAYS; i++, angle += angleIncrement)
        {
            Ray ray = castRay(player_pos_sf, angle, map, true);
            if (ray.hit)
            {
                ray.distance *= std::cos((player_pos[2] - angle) * PI / 180.0f);
                float wallHeight = (map.getCellsize() * ScreenH) / ray.distance;
                float walloffset = ScreenH / 2.0f - wallHeight / 2.0f;
                int textureX;
                if (ray.isHitVertical)
                {
                    textureX = ray.hitPosition.y - wall_texture.getSize().x * std::floor(ray.hitPosition.y / wall_texture.getSize().x);
                }
                else
                {
                    textureX = wall_texture.getSize().x * std::ceil(ray.hitPosition.x / wall_texture.getSize().x) - ray.hitPosition.x;
                }
                wall_sprite.setPosition({i * COLUMN_WIDTH, walloffset});
                wall_sprite.setTextureRect(sf::IntRect(
                    {textureX, 0},
                    {(int)(wall_texture.getSize().x / map.getCellsize()), (int)wall_texture.getSize().y}));
                wall_sprite.setScale({COLUMN_WIDTH, wallHeight / wall_texture.getSize().y});
                        
                if (wallHeight > ScreenH) wallHeight = ScreenH;
                
                float brightness = 1.0f - (ray.distance / maxRenderDistance);
                if (brightness < 0.0f) brightness = 0.01f;
                
                float shade = (ray.isHitVertical ? 0.8f : 1.0f) * brightness;
                fogAlpha = (ray.distance / maxFogDistance);
                if(fogAlpha > 1.0f){
                    fogAlpha = 1.0f;
                }
                column.setPosition({i * COLUMN_WIDTH, walloffset});
                column.setScale({COLUMN_WIDTH, wallHeight});
                column.setFillColor(sf::Color(fogColor.r, fogColor.g, fogColor.b, fogAlpha * 255));
                wall_sprite.setColor(sf::Color(255 * shade, 255 * shade, 255 * shade));
                target.draw(wall_sprite);
                target.draw(column);
            }
        }
    }
    catch (std::exception &e)
    {
        printf("%s", e.what());
    }
}

void Renderer::cast3DNewRay(sf::RenderTarget &target, Player &player, const Map &map)
{
    // Player Info
    auto player_pose = player.get_player_pose();
    sf::Vector2f playerPos(player_pose[0], player_pose[1]);
    float angle = player_pose[2] * PI / 180.0f;
    sf::Vector2f direction(std::cos(angle), std::sin(angle));
    const float fov = 60.0f;
    const float planeScale = std::tan(fov * PI / 360.0f);
    sf::Vector2f plane(
        -direction.y * planeScale,
        direction.x * planeScale
    );

    // Sky
    sf::RectangleShape rectangle(sf::Vector2f(ScreenW, ScreenH / 2.0f));
    rectangle.setFillColor(sf::Color(100, 170, 250));
    target.draw(rectangle);

    // Floor
   

    sf::VertexArray walls(sf::PrimitiveType::Triangles);
    const auto &grid = map.getGridColor();
    const float cellSize = map.getCellsize();
    const float texSize = static_cast<float>(wall_texture.getSize().x);
    const float maxDistance = MaxRayCastingDepth * cellSize;

    for (int x = 0; x < ScreenW; x++)
    {
        float cameraX = 2.0f * x / float(ScreenW) - 1.0f;

        sf::Vector2f rayPos = playerPos / cellSize;
        sf::Vector2f rayDir = direction + plane * cameraX;

        sf::Vector2f deltaDist;

        deltaDist.x = (rayDir.x == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.x);

        deltaDist.y = (rayDir.y == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.y);

        sf::Vector2i mapPos(
            (int)rayPos.x,
            (int)rayPos.y);

        sf::Vector2i step;
        sf::Vector2f sideDist;

        if (rayDir.x < 0)
        {
            step.x = -1;
            sideDist.x = (rayPos.x - mapPos.x) * deltaDist.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (mapPos.x + 1.0f - rayPos.x) * deltaDist.x;
        }

        if (rayDir.y < 0)
        {
            step.y = -1;
            sideDist.y = (rayPos.y - mapPos.y) * deltaDist.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (mapPos.y + 1.0f - rayPos.y) * deltaDist.y;
        }

        bool hit = false;
        bool vertical = false;

        while (!hit)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                mapPos.x += step.x;
                vertical = true;
            }
            else
            {
                sideDist.y += deltaDist.y;
                mapPos.y += step.y;
                vertical = false;
            }

            if (mapPos.x < 0 ||
                mapPos.y < 0 ||
                mapPos.y >= (int)grid.size() ||
                mapPos.x >= (int)grid[0].size())
                break;

            if (grid[mapPos.y][mapPos.x] != sf::Color::Black)
                hit = true;
        }

        if (!hit)
            continue;

        float perpWallDist;

        if (vertical)
            perpWallDist = sideDist.x - deltaDist.x;
        else
            perpWallDist = sideDist.y - deltaDist.y;

        perpWallDist = std::max(perpWallDist, 0.001f);

        float lineHeight = ScreenH / perpWallDist;

        float drawStart = (ScreenH - lineHeight) * 0.5f;
        float drawEnd   = (ScreenH + lineHeight) * 0.5f;

        float wallX;

        if (vertical)
            wallX = rayPos.y + perpWallDist * rayDir.y;
        else
            wallX = rayPos.x + perpWallDist * rayDir.x;

        wallX -= std::floor(wallX);

        int texX = (int)(wallX * texSize);

        if (vertical && rayDir.x > 0)
            texX = texSize - texX - 1;

        if (!vertical && rayDir.y < 0)
            texX = texSize - texX - 1;

        texX = std::clamp(texX, 0, (int)texSize - 1);

        float brightness = 1.0f - perpWallDist / maxDistance;
        brightness = std::clamp(brightness, 0.2f, 1.0f);

        if (vertical)
            brightness *= 0.75f;

        std::uint8_t c = static_cast<std::uint8_t>(255.0f * brightness);
        sf::Color color(c, c, c);

        float x0 = (float)x;
        float x1 = x0 + 1.0f;

        sf::Vector2f t0(texX, 0);
        sf::Vector2f t1(texX + 1, 0);
        sf::Vector2f t2(texX, texSize);
        sf::Vector2f t3(texX + 1, texSize);

        // Triangle 1
        walls.append({{x0, drawStart}, color, t0});
        walls.append({{x1, drawStart}, color, t1});
        walls.append({{x0, drawEnd}, color, t2});

        // Triangle 2
        walls.append({{x1, drawStart}, color, t1});
        walls.append({{x1, drawEnd}, color, t3});
        walls.append({{x0, drawEnd}, color, t2});
    }

    target.draw(walls, &wall_texture);
}

void Renderer::drawRays(sf::RenderTarget &target, Player &player, const Map &map)
{
    try
    {
        auto player_pos = player.get_player_pose();
        sf::Vector2f player_pos_sf = {player_pos[0], player_pos[1]};
        for (float angle = player_pos[2] - player_fov / 2.0f; angle < player_pos[2] + player_fov / 2.0; angle += 1)
        {
            Ray ray = castRay(player_pos_sf, angle, map, false);
            if (ray.hit)
            {
                sf::Vertex line[] = {
                    {player_pos_sf},
                    {ray.hitPosition}};
                target.draw(line, 2, sf::PrimitiveType::Lines);
            }
        }
    }
    catch (std::exception &e)
    {
        printf("%s", e.what());
    }
}