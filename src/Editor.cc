#include "Editor.h"
#include <SFML/Graphics/Sprite.hpp>

enum class WallIndex{
    Black       = -1,
    Red         = 0,
    Magenta     = 1,
    Green       = 2,
    White       = 3,
    Blue        = 4,
    Yellow      = 5,
    Cyan        = 6,
    Transparent = 7
};  

void Editor::init(sf::RenderWindow &window){view = window.getView();cell.setFillColor(sf::Color::Green);}

sf::Color getWallColor(WallIndex wall)
{
    switch (wall)
    {
        case WallIndex::Red:         return sf::Color::Red;
        case WallIndex::Magenta:     return sf::Color::Magenta;
        case WallIndex::Green:       return sf::Color::Green;
        case WallIndex::White:       return sf::Color::White;
        case WallIndex::Blue:        return sf::Color::Blue;
        case WallIndex::Yellow:      return sf::Color::Yellow;
        case WallIndex::Cyan:        return sf::Color::Cyan;
        case WallIndex::Transparent: return sf::Color::Transparent;
    }

    return sf::Color::Black;
}

void Editor::run(sf::RenderWindow &window, Map &map){
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("Save")){
                map.save("latest.map");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::Begin("Editing Options");
    ImGui::Text("Texture No. : ");
    ImGui::InputInt("##tex_no", &textureNo);

    int textureSize = Resources::walltextures.getSize().y;
    ImGui::Text("Preview :");
    ImGui::Image(
        sf::Sprite{
            Resources::walltextures,
            sf::IntRect{{textureNo * textureSize, 0}, {textureSize, textureSize}},
        },
        sf::Vector2f({100.0f, 100.0f}));
    ImGui::End();
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
        if(isFirstMouse){
            lastMousePos = mousePos;
            isFirstMouse = false;
        } else {            
            auto mouseDelta = mousePos - lastMousePos;
            view.setCenter(view.getCenter() - (sf::Vector2f)mouseDelta);
            sf::Mouse::setPosition(lastMousePos, window);
        }
        window.setMouseCursorVisible(false);
    } else {
        isFirstMouse = true;
        window.setMouseCursorVisible(true);
    }
    if(!ImGui::GetIO().WantCaptureMouse){
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        sf::Vector2i mapPos = (sf::Vector2i)(worldPos/ map.getCellsize());
        cell.setSize(sf::Vector2f(map.getCellsize(), map.getCellsize()));
        cell.setPosition((sf::Vector2f)mapPos * map.getCellsize());
        window.setView(view);
        window.draw(cell);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            map.SetMap(
            mapPos.x,
            mapPos.y,
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift)
                ? getWallColor(WallIndex::Black)
                : getWallColor(WallIndex::White)
            );
        }
    }
}

void Editor::handleEvent(const sf::Event &event){
    if(const auto* mouse_scroll = event.getIf<sf::Event::MouseWheelScrolled>()){
        float zoom = 1.0f - 0.1f * mouse_scroll->delta;
        view.zoom(zoom);
    }
}