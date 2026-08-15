#include "Editor.h"

void Editor::init(sf::RenderWindow &window){view = window.getView();cell.setFillColor(sf::Color::Green);}

void Editor::run(sf::RenderWindow &window, Map &map){
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
        if(isFirstMouse){
            lastMousePos = mousePos;
            isFirstMouse = false;
        } else {            
            auto mouseDelta = mousePos - lastMousePos;
            view.setCenter(view.getCenter() - (sf::Vector2f)mouseDelta);
            // window.setView(view);
            sf::Mouse::setPosition(lastMousePos, window);
        }
        window.setMouseCursorVisible(false);
    } else {
        isFirstMouse = true;
        window.setMouseCursorVisible(true);
    }
    if(ImGui::GetIO().WantCaptureMouse){
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        sf::Vector2i mapPos = (sf::Vector2i)(worldPos/ map.getCellsize());
        cell.setSize(sf::Vector2f(map.getCellsize(), map.getCellsize()));
        cell.setPosition((sf::Vector2f)mapPos * map.getCellsize());
        window.draw(cell);
        window.setView(view);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            map.SetMap(
            mapPos.x,
            mapPos.y,
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift)
                ? sf::Color::Black
                : sf::Color::White
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