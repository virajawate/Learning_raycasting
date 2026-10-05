#include "GLRenderer.h"

#ifndef _WIN32

#include <SFML/OpenGL.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "Resources.h"
#include "Render.h"

namespace {
constexpr float PI = 3.14159265358979323846f;

int textureIndex(sf::Color color) {
    if (color == sf::Color::White) return 0;
    if (color == sf::Color::Cyan) return 1;
    if (color == sf::Color::Red) return 2;
    if (color == sf::Color::Green) return 3;
    if (color == sf::Color::Yellow) return 4;
    if (color == sf::Color(255, 26, 0, 255)) return 5;
    return -1;
}

void texturedQuad(float x0, float y0, float x1, float y1,
                  float u0, float v0, float u1, float v1) {
    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex2f(x0, y0);
    glTexCoord2f(u1, v0); glVertex2f(x1, y0);
    glTexCoord2f(u1, v1); glVertex2f(x1, y1);
    glTexCoord2f(u0, v1); glVertex2f(x0, y1);
    glEnd();
}

void drawGroundPlane(unsigned int texture,
                     const sf::Vector2f& player, const sf::Vector2f& direction,
                     const sf::Vector2f& plane, bool ceiling) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor4ub(255, 255, 255, 255);

    const int startY = ceiling ? 0 : static_cast<int>(ScreenH / 2 + 1);
    const int endY = ceiling ? static_cast<int>(ScreenH / 2) : static_cast<int>(ScreenH);
    for (int y = startY; y < endY; ++y) {
        const float rowDistance = CAMERA_Z /
            static_cast<float>(ceiling ? (ScreenH / 2 - y) : (y - ScreenH / 2));
        const sf::Vector2f leftRay = direction - plane;
        const sf::Vector2f rightRay = direction + plane;
        const float stepDivisor = ceiling ? static_cast<float>(ScreenH) : static_cast<float>(ScreenW);
        const sf::Vector2f step{
            rowDistance * (rightRay.x - leftRay.x) / stepDivisor,
            rowDistance * (rightRay.y - leftRay.y) / stepDivisor
        };
        const sf::Vector2f start = player + rowDistance * leftRay;
        const sf::Vector2f finish = start + step * static_cast<float>(ScreenW);
        const float vOffset = ceiling ? 1.0f : 0.0f;

        glBegin(GL_QUADS);
        glTexCoord2f(start.x, start.y + vOffset); glVertex2f(0.0f, static_cast<float>(y));
        glTexCoord2f(finish.x, finish.y + vOffset); glVertex2f(static_cast<float>(ScreenW), static_cast<float>(y));
        glTexCoord2f(finish.x, finish.y + vOffset); glVertex2f(static_cast<float>(ScreenW), static_cast<float>(y + 1));
        glTexCoord2f(start.x, start.y + vOffset); glVertex2f(0.0f, static_cast<float>(y + 1));
        glEnd();
    }
}
}

GLRenderer::GLRenderer() = default;

GLRenderer::~GLRenderer() {
    if (textures[0] != 0) {
        glDeleteTextures(static_cast<GLsizei>(textures.size()), textures.data());
    }
}

void GLRenderer::init() {
    const sf::Image atlas = Resources::walltextures.copyToImage();
    const auto atlasSize = atlas.getSize();
    if (atlasSize.x == 0 || atlasSize.y == 0 || atlasSize.x % textures.size() != 0) {
        throw std::runtime_error("Wall texture atlas must contain 11 equal-width tiles.");
    }
    textureSize = atlasSize.y;
    if (atlasSize.x / textures.size() != textureSize) {
        throw std::runtime_error("Wall texture atlas tiles must be square.");
    }

    glGenTextures(static_cast<GLsizei>(textures.size()), textures.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (std::size_t tile = 0; tile < textures.size(); ++tile) {
        std::vector<std::uint8_t> pixels(textureSize * textureSize * 4);
        for (unsigned int y = 0; y < textureSize; ++y) {
            for (unsigned int x = 0; x < textureSize; ++x) {
                const sf::Color color = atlas.getPixel(
                    {static_cast<unsigned int>(tile) * textureSize + x, y});
                const std::size_t offset = (static_cast<std::size_t>(y) * textureSize + x) * 4;
                pixels[offset] = color.r;
                pixels[offset + 1] = color.g;
                pixels[offset + 2] = color.b;
                pixels[offset + 3] = color.a;
            }
        }
        glBindTexture(GL_TEXTURE_2D, textures[tile]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }
    glDisable(GL_TEXTURE_2D);
}

void GLRenderer::beginFrame(sf::RenderWindow& window) {
    const sf::Vector2u size = window.getSize();
    glViewport(0, 0, static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, ScreenW, ScreenH, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void GLRenderer::drawGame(sf::RenderWindow& window, const Player& player, const Map& map) {
    beginFrame(window);
    const auto pose = player.get_player_pose();
    const float angle = pose[2] * PI / 180.0f;
    const sf::Vector2f direction(std::cos(angle), std::sin(angle));
    const float planeScale = std::tan(45.0f * PI / 180.0f);
    const sf::Vector2f plane(-direction.y * planeScale, direction.x * planeScale);
    const sf::Vector2f playerPosition(pose[0], pose[1]);

    drawGroundPlane(textures[6], playerPosition, direction, plane, true);
    drawGroundPlane(textures[7], playerPosition, direction, plane, false);

    glEnable(GL_TEXTURE_2D);
    const float maxDistance = 128.0f;
    for (int x = 0; x < static_cast<int>(ScreenW); ++x) {
        const float cameraX = 2.0f * x / static_cast<float>(ScreenW) - 1.0f;
        const sf::Vector2f rayDirection = direction + plane * cameraX;
        const float deltaX = rayDirection.x == 0.0f ? INFINITY : std::abs(1.0f / rayDirection.x);
        const float deltaY = rayDirection.y == 0.0f ? INFINITY : std::abs(1.0f / rayDirection.y);
        sf::Vector2i mapPosition(static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y));
        const sf::Vector2i step(rayDirection.x < 0.0f ? -1 : 1,
                                rayDirection.y < 0.0f ? -1 : 1);
        float sideX = (rayDirection.x < 0.0f ? playerPosition.x - mapPosition.x
                                             : mapPosition.x + 1.0f - playerPosition.x) * deltaX;
        float sideY = (rayDirection.y < 0.0f ? playerPosition.y - mapPosition.y
                                             : mapPosition.y + 1.0f - playerPosition.y) * deltaY;
        bool vertical = false;
        sf::Color wallColor = sf::Color::Black;

        for (std::size_t depth = 0; depth < 128; ++depth) {
            if (sideX < sideY) {
                sideX += deltaX;
                mapPosition.x += step.x;
                vertical = true;
            } else {
                sideY += deltaY;
                mapPosition.y += step.y;
                vertical = false;
            }
            wallColor = map.getGridCell(mapPosition.x, mapPosition.y);
            if (wallColor != sf::Color::Black) break;
        }
        if (wallColor == sf::Color::Black) continue;

        int tile = textureIndex(wallColor);
        if (tile < 0) tile = 6;
        const float distance = std::max(vertical ? sideX - deltaX : sideY - deltaY, 0.001f);
        const float wallHeight = ScreenH / distance;
        const float top = (ScreenH - wallHeight) * 0.5f;
        const float bottom = (ScreenH + wallHeight) * 0.5f;
        float wallX = vertical
            ? playerPosition.y + distance * rayDirection.y
            : playerPosition.x + distance * rayDirection.x;
        wallX -= std::floor(wallX);
        float textureX = wallX;
        if ((!vertical && rayDirection.x > 0.0f) || (vertical && rayDirection.y < 0.0f)) {
            textureX = 1.0f - textureX;
        }
        float brightness = std::clamp(1.0f - distance / maxDistance, 0.2f, 1.0f);
        if (!vertical) brightness *= 0.75f;
        const auto shade = static_cast<GLubyte>(255.0f * brightness);

        glBindTexture(GL_TEXTURE_2D, textures[tile]);
        glColor4ub(shade, shade, shade, 255);
        texturedQuad(static_cast<float>(x), top, static_cast<float>(x + 1), bottom,
                     textureX, 0.0f, textureX, 1.0f);
    }
    glColor4ub(255, 255, 255, 255);
    glDisable(GL_TEXTURE_2D);
}

void GLRenderer::drawTexturedMap(const Map& map, float cellSize) {
    const auto grid = map.getGridColor();
    glEnable(GL_TEXTURE_2D);
    for (std::size_t y = 0; y < grid.size(); ++y) {
        for (std::size_t x = 0; x < grid[y].size(); ++x) {
            const int tile = textureIndex(grid[y][x]);
            if (tile < 0) continue;
            glBindTexture(GL_TEXTURE_2D, textures[tile]);
            glColor4ub(255, 255, 255, 255);
            texturedQuad(x * cellSize, y * cellSize, (x + 1) * cellSize, (y + 1) * cellSize,
                         0.0f, 0.0f, 1.0f, 1.0f);
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void GLRenderer::drawColorMap(const Map& map, float cellSize) {
    const auto grid = map.getGridColor();
    for (std::size_t y = 0; y < grid.size(); ++y) {
        for (std::size_t x = 0; x < grid[y].size(); ++x) {
            const sf::Color color = grid[y][x];
            glColor4ub(color.r, color.g, color.b, color.a);
            const float left = x * cellSize + cellSize * 0.025f;
            const float top = y * cellSize + cellSize * 0.025f;
            const float right = left + cellSize * 0.95f;
            const float bottom = top + cellSize * 0.95f;
            glBegin(GL_QUADS);
            glVertex2f(left, top);
            glVertex2f(right, top);
            glVertex2f(right, bottom);
            glVertex2f(left, bottom);
            glEnd();
        }
    }
}

void GLRenderer::drawPlayer(const Player& player) {
    const auto pose = player.get_player_pose();
    const float x = pose[0] * 50.0f;
    const float y = pose[1] * 50.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(0, 255, 0, 125);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(pose[2] - 45.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(150.0f, 0.0f);
    glVertex2f(150.0f, 150.0f);
    glVertex2f(0.0f, 150.0f);
    glEnd();
    glPopMatrix();

    glColor4ub(0, 0, 255, 255);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + 5.0f, y + 5.0f);
    for (int i = 0; i <= 32; ++i) {
        const float angle = 2.0f * PI * i / 32.0f;
        glVertex2f(x + 5.0f + 25.0f * std::cos(angle),
                   y + 5.0f + 25.0f * std::sin(angle));
    }
    glEnd();
    glDisable(GL_BLEND);
    glColor4ub(255, 255, 255, 255);
}

void GLRenderer::drawEditor(sf::RenderWindow& window, const Map& map, const Player& player,
                            const Editor& editor, float cellSize, bool textured) {
    beginFrame(window);
    const sf::View view = editor.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(center.x - viewSize.x / 2.0, center.x + viewSize.x / 2.0,
            center.y + viewSize.y / 2.0, center.y - viewSize.y / 2.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (textured) drawTexturedMap(map, cellSize);
    else drawColorMap(map, cellSize);
    drawPlayer(player);
    if (editor.hasPreview()) {
        const sf::Vector2f position = editor.getCellPosition();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(0, 255, 0, 255);
        glBegin(GL_QUADS);
        glVertex2f(position.x, position.y);
        glVertex2f(position.x + cellSize, position.y);
        glVertex2f(position.x + cellSize, position.y + cellSize);
        glVertex2f(position.x, position.y + cellSize);
        glEnd();
        glDisable(GL_BLEND);
        glColor4ub(255, 255, 255, 255);
    }
}

#else

GLRenderer::GLRenderer() = default;
GLRenderer::~GLRenderer() = default;
void GLRenderer::init() {}
void GLRenderer::drawGame(sf::RenderWindow&, const Player&, const Map&) {}
void GLRenderer::drawEditor(sf::RenderWindow&, const Map&, const Player&, const Editor&, float, bool) {}

#endif
