#include "Render.h"
#include "Resources.h"

// Renderer::Renderer() : wall_texture(), wall_sprite(wall_texture) {}
Renderer::Renderer() = default;

void Renderer::init()
{
    if (!floorBuffer.resize({ScreenW, ScreenH}))
        throw std::runtime_error("Failed to create floor buffer.");

    // if (!wall_texture.loadFromFile(wall_texture_file))
    //     throw std::runtime_error("Failed to load " + wall_texture_file);
    
    if (!sky_texture.loadFromFile(sky_texture_file))
        throw std::runtime_error("Failed to load " + sky_texture_file);

    if (!floor_texture.loadFromFile(floor_texture_file))
        throw std::runtime_error("Failed to load " + floor_texture_file);

    // if (Resources::wall_texture.getSize().x != Resources::wall_texture.getSize().y)
    //     throw std::runtime_error("Wall texture must be square.");

    if (floor_texture.getSize().x != floor_texture.getSize().y)
        throw std::runtime_error("Floor texture must be square.");

    sky_texture.setRepeated(true);
    // wall_sprite.emplace(Resources::wall_texture);
    floorSprite.emplace(floorBuffer);

    std::cout << "Texture Files Loaded\n";
    std::cout << "Initialization Complete.\n";
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
    sf::Vector2f vplayer_loc, hplayer_loc, offset;
    if (cos(angle) > 0.001f)
    {
        vplayer_loc.x = std::floor(start.x / cellsize) * cellsize + cellsize;
        offset.x = cellsize;
    }
    else if (cos(angle) < -0.001f)
    {
        vplayer_loc.x = std::floor(start.x / cellsize) * cellsize - 0.01f;
        offset.x = -cellsize;
    }
    else
    {
        vdof = MaxRayCastingDepth;
    }
    vplayer_loc.y = (start.x - vplayer_loc.x) * vTan + start.y;
    offset.y = -(offset.x * vTan);
    unsigned int mapX = (unsigned int)(vplayer_loc.x / cellsize);
    unsigned int mapY = (unsigned int)(vplayer_loc.y / cellsize);

    for (; vdof < MaxRayCastingDepth; vdof++)
    {
        mapX = (unsigned int)(vplayer_loc.x / cellsize);
        mapY = (unsigned int)(vplayer_loc.y / cellsize);
        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            auto det_x = vplayer_loc.x - start.x;
            auto det_y = vplayer_loc.y - start.y;
            vdist = std::hypotf(det_x, det_y);
            vMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        vplayer_loc += offset;
    }

    if (sin(angle) < -0.001f)
    {
        hplayer_loc.y = std::floor(start.y / cellsize) * cellsize - 0.01f;
        offset.y = -cellsize;
    }
    else if (sin(angle) > 0.001f)
    {
        hplayer_loc.y = std::floor(start.y / cellsize) * cellsize + cellsize;
        offset.y = cellsize;
    }
    else
    {
        hdof = MaxRayCastingDepth;
    }
    hplayer_loc.x = (start.y - hplayer_loc.y) * hTan + start.x;
    offset.x = -offset.y * hTan;
    mapX = (unsigned int)std::floor(vplayer_loc.x / cellsize);
    mapY = (unsigned int)std::floor(hplayer_loc.y / cellsize);

    for (; hdof < MaxRayCastingDepth; hdof++)
    {
        mapX = (unsigned int)(hplayer_loc.x / cellsize);
        mapY = (unsigned int)(hplayer_loc.y / cellsize);

        if (mapY < rows && mapX < cols && grid[mapY][mapX] != sf::Color::Black)
        {
            hit = true;
            auto det_x = hplayer_loc.x - start.x;
            auto det_y = hplayer_loc.y - start.y;
            hdist = std::hypotf(det_x, det_y);
            hMapPos = sf::Vector2u({mapX, mapY});
            break;
        }
        hplayer_loc += offset;
    }
    bool verticle = (vdist < hdist);
    return Ray{(verticle ? vplayer_loc : hplayer_loc), (verticle ? hMapPos : vMapPos), std::min(vdist, hdist), hit, verticle};
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
                wall_sprite->setPosition({i * COLUMN_WIDTH, walloffset});

                wall_sprite->setTextureRect(sf::IntRect(
                    {textureX, 0},
                    {
                        static_cast<int>(wall_texture.getSize().x / map.getCellsize()),
                        static_cast<int>(wall_texture.getSize().y)
                    }));

                wall_sprite->setScale({
                    COLUMN_WIDTH,
                    wallHeight / static_cast<float>(wall_texture.getSize().y)
                });

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
                wall_sprite->setColor(sf::Color(
                    static_cast<std::uint8_t>(255 * shade),
                    static_cast<std::uint8_t>(255 * shade),
                    static_cast<std::uint8_t>(255 * shade)));

                target.draw(*wall_sprite);
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
    const float fov = 60.0f; 
    // Map Info
    const auto &grid = map.getGridColor();
    const float cellSize = map.getCellsize();
    const float texSize = static_cast<float>(wall_texture.getSize().x);
    const float maxDistance = MaxRayCastingDepth * cellSize;

    // Player Info
    auto player_pose = player.get_player_pose();
    sf::Vector2f playerPos(player_pose[0], player_pose[1]);
    float angle = player_pose[2] * PI / 180.0f;
    sf::Vector2f direction(std::cos(angle), std::sin(angle));
    const float planeScale = std::tan(fov * PI / 360.0f);
    sf::Vector2f plane(
        -direction.y * planeScale,
        direction.x * planeScale
    );
    sf::Vector2f player_loc = playerPos / cellSize;

    // Sky
    int xOffset = ScreenW / PLAYER_TURN_SPEED * player_pose[2];
    while(xOffset < 0){xOffset += sky_texture.getSize().x;}

    sf::Vertex sky[] = {
        sf::Vertex({sf::Vector2f{0.0f, 0.0f}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset), 0.0f}}),
        sf::Vertex({sf::Vector2f{0.0f, ScreenH}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset), static_cast<float>(sky_texture.getSize().y)}}),
        sf::Vertex({sf::Vector2f{ScreenW, ScreenH}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset + sky_texture.getSize().x), static_cast<float>(sky_texture.getSize().y)}}),
        sf::Vertex({sf::Vector2f{ScreenW, 0.0f}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset + sky_texture.getSize().x), 0.0f}}),
    };
    target.draw(sky, 4, sf::PrimitiveType::TriangleFan, sf::RenderStates(&sky_texture));

    // Floor
    std::vector<uint8_t> floorPixels(ScreenW * ScreenH * 4);

    for(size_t y= ScreenH / 2; y < ScreenH; y++){
        if(y == ScreenH / 2) continue;
        sf::Vector2f rayDirLeft{direction - plane}, rayDirRight{direction + plane};
        float rowDistance = CAMERA_Z / ((float)y - ScreenH / 2);
        sf::Vector2f floorStep = {rowDistance * (rayDirRight - rayDirLeft) / static_cast<float>(ScreenW)};
        sf::Vector2f floor = player_loc + rowDistance * rayDirLeft;
        for (size_t x = 0; x<ScreenW; x++){
            sf::Vector2i cell{floor};
            float textureSize = floor_texture.getSize().x;
            sf::Vector2u texCoords{textureSize * (floor - (sf::Vector2f)cell)};
            texCoords.x &= (unsigned)textureSize - 1;
            texCoords.y &= (unsigned)textureSize - 1;
            
            sf::Color color= floor_texture.getPixel(texCoords);
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 0] = color.r;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 1] = color.g;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 2] = color.b;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 3] = color.a;
            floor += floorStep;
        }
    }
    floorBuffer.update(floorPixels.data());
    floorSprite->setTexture(floorBuffer);
    // Drawing floorSprite caused overload resolution error; skip drawing here.
    if(floorSprite) target.draw(*floorSprite);
    
    sf::VertexArray walls(sf::PrimitiveType::Triangles);
    for (int x = 0; x < ScreenW; x++)
    {
        float cameraX = 2.0f * x / float(ScreenW) - 1.0f;
        sf::Vector2f rayDir = direction + plane * cameraX;
        sf::Vector2f deltaDist;

        deltaDist.x = (rayDir.x == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.x);

        deltaDist.y = (rayDir.y == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.y);

        sf::Vector2i mapPos(
            (int)player_loc.x,
            (int)player_loc.y);

        sf::Vector2i step;
        sf::Vector2f sideDist;

        if (rayDir.x < 0)
        {
            step.x = -1;
            sideDist.x = (player_loc.x - mapPos.x) * deltaDist.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (mapPos.x + 1.0f - player_loc.x) * deltaDist.x;
        }

        if (rayDir.y < 0)
        {
            step.y = -1;
            sideDist.y = (player_loc.y - mapPos.y) * deltaDist.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (mapPos.y + 1.0f - player_loc.y) * deltaDist.y;
        }

        bool hit = false;
        bool verticle = false;

        while (!hit)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                mapPos.x += step.x;
                verticle = true;
            }
            else
            {
                sideDist.y += deltaDist.y;
                mapPos.y += step.y;
                verticle = false;
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

        if (verticle)
            perpWallDist = sideDist.x - deltaDist.x;
        else
            perpWallDist = sideDist.y - deltaDist.y;

        perpWallDist = std::max(perpWallDist, 0.001f);

        float lineHeight = ScreenH / perpWallDist;

        float drawStart = (ScreenH - lineHeight) * 0.5f;
        float drawEnd   = (ScreenH + lineHeight) * 0.5f;

        float wallX;

        if (verticle)
            wallX = player_loc.y + perpWallDist * rayDir.y;
        else
            wallX = player_loc.x + perpWallDist * rayDir.x;

        wallX -= std::floor(wallX);

        int texX = (int)(wallX * texSize);

        if (verticle && rayDir.x > 0)
            texX = texSize - texX - 1;

        if (!verticle && rayDir.y < 0)
            texX = texSize - texX - 1;

        texX = std::clamp(texX, 0, (int)texSize - 1);

        float brightness = 1.0f - perpWallDist / maxDistance;
        brightness = std::clamp(brightness, 0.2f, 1.0f);

        if (verticle)
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
    
    // target.draw(floorPixel, &floor_texture);
    target.draw(walls, sf::RenderStates{&wall_texture});
}

void Renderer::cast3DNewRayGUI(sf::RenderTarget &target, Player &player, const Map &map)
{
    const float fov = 60.0f; 
    // Map Info
    const auto &grid = map.getGridColor();
    const float cellSize = map.getCellsize();
    const float texSize = static_cast<float>(wall_texture.getSize().x);
    const float maxDistance = MaxRayCastingDepth * cellSize;

    // Player Info
    auto player_pose = player.get_player_pose();
    sf::Vector2f playerPos(player_pose[0], player_pose[1]);
    float angle = player_pose[2] * PI / 180.0f;
    sf::Vector2f direction(std::cos(angle), std::sin(angle));
    const float planeScale = std::tan(fov * PI / 360.0f);
    sf::Vector2f plane(
        -direction.y * planeScale,
        direction.x * planeScale
    );
    sf::Vector2f player_loc = playerPos / cellSize;

    // Sky
    int xOffset = ScreenW / PLAYER_TURN_SPEED * player_pose[2];
    while(xOffset < 0){xOffset += sky_texture.getSize().x;}

    sf::Vertex sky[] = {
        sf::Vertex({sf::Vector2f{0.0f, 0.0f}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset), 0.0f}}),
        sf::Vertex({sf::Vector2f{0.0f, ScreenH}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset), static_cast<float>(sky_texture.getSize().y)}}),
        sf::Vertex({sf::Vector2f{ScreenW, ScreenH}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset + sky_texture.getSize().x), static_cast<float>(sky_texture.getSize().y)}}),
        sf::Vertex({sf::Vector2f{ScreenW, 0.0f}, sf::Color::White, sf::Vector2f{static_cast<float>(xOffset + sky_texture.getSize().x), 0.0f}}),
    };
    target.draw(sky, 4, sf::PrimitiveType::TriangleFan, sf::RenderStates(&sky_texture));

    // Floor
    std::vector<uint8_t> floorPixels(ScreenW * ScreenH * 4);

    for(size_t y= ScreenH / 2; y < ScreenH; y++){
        if(y == ScreenH / 2) continue;
        sf::Vector2f rayDirLeft{direction - plane}, rayDirRight{direction + plane};
        float rowDistance = CAMERA_Z / ((float)y - ScreenH / 2);
        sf::Vector2f floorStep = {rowDistance * (rayDirRight - rayDirLeft) / static_cast<float>(ScreenW)};
        sf::Vector2f floor = player_loc + rowDistance * rayDirLeft;
        for (size_t x = 0; x<ScreenW; x++){
            sf::Vector2i cell{floor};
            float textureSize = floor_texture.getSize().x;
            sf::Vector2u texCoords{textureSize * (floor - (sf::Vector2f)cell)};
            texCoords.x &= (unsigned)textureSize - 1;
            texCoords.y &= (unsigned)textureSize - 1;
            
            sf::Color color= floor_texture.getPixel(texCoords);
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 0] = color.r;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 1] = color.g;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 2] = color.b;
            floorPixels[(x + y * (size_t)ScreenW) * 4 + 3] = color.a;
            floor += floorStep;
        }
    }
    floorBuffer.update(floorPixels.data());
    floorSprite->setTexture(floorBuffer);
    // Drawing floorSprite caused overload resolution error; skip drawing here.
    if(floorSprite) target.draw(*floorSprite);
    
    sf::VertexArray walls(sf::PrimitiveType::Triangles);
    for (int x = 0; x < ScreenW; x++)
    {
        float cameraX = 2.0f * x / float(ScreenW) - 1.0f;
        sf::Vector2f rayDir = direction + plane * cameraX;
        sf::Vector2f deltaDist;

        deltaDist.x = (rayDir.x == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.x);

        deltaDist.y = (rayDir.y == 0.0f)
                          ? std::numeric_limits<float>::infinity()
                          : std::abs(1.0f / rayDir.y);

        sf::Vector2i mapPos(
            (int)player_loc.x,
            (int)player_loc.y);

        sf::Vector2i step;
        sf::Vector2f sideDist;

        if (rayDir.x < 0)
        {
            step.x = -1;
            sideDist.x = (player_loc.x - mapPos.x) * deltaDist.x;
        }
        else
        {
            step.x = 1;
            sideDist.x = (mapPos.x + 1.0f - player_loc.x) * deltaDist.x;
        }

        if (rayDir.y < 0)
        {
            step.y = -1;
            sideDist.y = (player_loc.y - mapPos.y) * deltaDist.y;
        }
        else
        {
            step.y = 1;
            sideDist.y = (mapPos.y + 1.0f - player_loc.y) * deltaDist.y;
        }

        int hit{}, verticle{};
        size_t depth = 0;
        while (hit == 0 && depth < MaxRayCastingDepth)
        {
            if (sideDist.x < sideDist.y)
            {
                sideDist.x += deltaDist.x;
                mapPos.x += step.x;
                verticle = false;
            }
            else
            {
                sideDist.y += deltaDist.y;
                mapPos.y += step.y;
                verticle = true;
            }

            if (mapPos.x < 0 ||
                mapPos.y < 0 ||
                mapPos.y >= (int)grid.size() ||
                mapPos.x >= (int)grid[0].size())
                break;

            if (grid[mapPos.y][mapPos.x] != sf::Color::Black)
                hit = true;
            depth++;
        }

        if (!hit) continue;

        float perpWallDist;

        if (!verticle)
            perpWallDist = sideDist.x - deltaDist.x;
        else
            perpWallDist = sideDist.y - deltaDist.y;

        perpWallDist = std::max(perpWallDist, 0.001f);

        float lineHeight = ScreenH / perpWallDist;

        float drawStart = (ScreenH - lineHeight) * 0.5f;
        float drawEnd   = (ScreenH + lineHeight) * 0.5f;

        float wallX;

        if (!verticle)
            wallX = player_loc.y + perpWallDist * rayDir.y;
        else
            wallX = player_loc.x + perpWallDist * rayDir.x;

        wallX -= std::floor(wallX);

        int texX = (int)(wallX * texSize);

        if (!verticle && rayDir.x > 0)
            texX = texSize - texX - 1;

        if (verticle && rayDir.y < 0)
            texX = texSize - texX - 1;

        texX = std::clamp(texX, 0, (int)texSize - 1);

        float brightness = 1.0f - perpWallDist / maxDistance;
        brightness = std::clamp(brightness, 0.2f, 1.0f);

        if (!verticle)
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
    
    // target.draw(floorPixel, &floor_texture);
    sf::RenderStates states{&Resources::walltextures};
    target.draw(walls, states);
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