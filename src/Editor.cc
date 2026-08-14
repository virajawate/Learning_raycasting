#include "Editor.h"

void Editor::init(sf::RenderWindow &window){view = window.getView();}

void Editor::run(sf::RenderWindow & window){
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
        if(isFirstMouse){
            lastMousePos = sf::Mouse::getPosition(window);
            isFirstMouse = false;
        } else {
            
            sf::Vector2i mouseDelta = mousePos - lastMousePos;
            
            view.setCenter(view.getCenter() - (sf::Vector2f)mouseDelta);
            window.setView(view);
            sf::Mouse::setPosition(lastMousePos, window);
        }
        window.setMouseCursorVisible(false);
    } else {
        isFirstMouse = true;
        window.setMouseCursorVisible(true);
    }
    sf::Vector2i worldPos = window.mapPixelToCoords(mousePos);
    window.setView(view);
}

void Editor::handleEvent(const sf::Event &event){
    if(const auto* mouse_scroll = event.getIf<sf::Event::MouseWheelScrolled>()){
        float zoom = 1.0f - 0.1f * mouse_scroll->delta;
        view.zoom(zoom);
    }
}