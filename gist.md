```cpp
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

        printf("\n=============================\n");
        printf("Player Position : (%.2f, %.2f)\n", player_pos[0], player_pos[1]);
        printf("Player Angle    : %.2f deg\n", player_pos[2]);

        float angle = player_pos[2] - player_fov / 2.0f;
        float angleIncrement = player_fov / static_cast<float>(NUM_RAYS);

        printf("Start Angle     : %.2f\n", angle);
        printf("Angle Increment : %.4f\n", angleIncrement);

        for(size_t i = 0; i < NUM_RAYS; i++, angle += angleIncrement){

            printf("\nRay %zu -----------------\n", i);
            printf("Ray Angle : %.2f\n", angle);

            Ray ray = castRay(player_pos_sf, angle, map);

            printf("Hit      : %d\n", ray.hit);
            printf("Distance : %f\n", ray.distance);
            printf("Hit Pos  : (%f, %f)\n", ray.hitPosition.x, ray.hitPosition.y);

            if (!ray.hit)
                continue;

            float correction = std::cos((player_pos[2] - angle) * PI / 180.0f);
            printf("Cos Correction : %f\n", correction);

            ray.distance *= correction;

            printf("Corrected Dist : %f\n", ray.distance);

            if (!std::isfinite(ray.distance) || ray.distance <= 0.0f){
                printf("Invalid distance!\n");
                continue;
            }

            float wallHeight = (map.getCellsize() * ScreenH) / ray.distance;

            printf("Wall Height : %f\n", wallHeight);

            if (wallHeight > ScreenH)
                wallHeight = ScreenH;

            float walloffset = ScreenH / 2.0f - wallHeight / 2.0f;

            printf("Wall Offset : %f\n", walloffset);

            sf::RectangleShape column(sf::Vector2f(COLUMN_WIDTH, wallHeight));
            column.setPosition({i * COLUMN_WIDTH, walloffset});

            target.draw(column);
        }
    }
    catch (std::exception &e){
        printf("Exception: %s\n", e.what());
    }
}
```