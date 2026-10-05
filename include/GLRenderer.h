#ifndef _GL_RENDERER_H
#define _GL_RENDERER_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <array>
#include "Editor.h"
#include "Map.h"
#include "Player.h"
#include "Sprite.h"

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    void init();
    void drawGame(sf::RenderWindow& window, const Player& player, const Map& map, const std::vector<Sprites> &sprites);
    void drawEditor(sf::RenderWindow& window, const Map& map, const Player& player,
                    const Editor& editor, float cellSize, bool textured);

private:
    void beginFrame(sf::RenderWindow& window);
    void drawPlayer(const Player& player);
    void drawTexturedMap(const Map& map, float cellSize);
    void drawColorMap(const Map& map, float cellSize);

    std::array<unsigned int, 11> textures{};
    unsigned int textureSize{};
};

#endif
