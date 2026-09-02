#include <algorithm>
#include "Editor.h"
#include <ImGuiFileDialog.h>
#include <SFML/Graphics/Sprite.hpp>

void Editor::init(sf::RenderWindow &window){
    view = window.getView();
    cell.setFillColor(sf::Color::Green);
}

void Editor::run(sf::RenderWindow &window, Map &map){
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("Open")){
                ImGuiFileDialog::Instance()->OpenDialog("OpenDialog", "Open", ".map");
            }
            if(ImGui::MenuItem("Save")){
                map.save("latest.map");
                map.saveImage("latest.png");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if(ImGuiFileDialog::Instance()->Display("OpenDialog")){
        if(ImGuiFileDialog::Instance()->IsOk()){
            std::cout<<"Here\n";
        }
        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::Begin("Wall Texture Options");
    ImGui::Text("Texture No. : ");
    ImGui::InputInt("##tex_no", &textureNo);
    textureNo = std::clamp(textureNo, 0, 4);
    auto SelectedColor = getColor(textureNo);
    int textureSize = Resources::walltextures.getSize().y;
    ImGui::Text("Preview :");
    ImGui::Image(
        sf::Sprite{
            Resources::walltextures,
            sf::IntRect{
                {textureNo * textureSize, 0}, 
                {textureSize, textureSize}
            },
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
                ? sf::Color::Black : SelectedColor
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

sf::Color Editor::getColor(int textureNo){
    switch (textureNo)
    {
    case 0:
        return sf::Color::White;
        break;
    case 1:
        return sf::Color::Cyan;
        break;
    case 2:
        return sf::Color::Red;
        break;
    case 3:
        return sf::Color::Green;
        break;
    case 4:
        return sf::Color::Yellow;
        break;
    default:
        return sf::Color::Black;
        break;
    }
}