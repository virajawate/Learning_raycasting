#include "Player.h"

#include <math.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
constexpr float PI = 3.1427;
constexpr float TURN_SPEED = 50.0f;
constexpr float MOVE_SPEED = 50.0f;

std::vector<float> Player::get_player_pose(){
    /* Player Pose
        Pose -> X
        Pose -> Y
        Pose -> Orientation
    */

    std::vector<float> player_pose;
    player_pose.push_back(position.x);
    player_pose.push_back(position.y);
    player_pose.push_back(angle);
    return player_pose;
}

void Player::draw(sf::RenderTarget &target){
    sf::CircleShape circle(player_size);
    circle.setPosition({position.x-player_size, position.y-player_size});
    circle.setFillColor(sf::Color::Blue);
    sf::RectangleShape line(sf::Vector2f(150.0f, 150.0f));
    line.setPosition(position);
    line.setRotation(sf::degrees(angle-45));
    line.setFillColor(sf::Color::Green);
    target.draw(line);
    target.draw(circle);
}

void Player::update(float deltaTime){
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
        angle += TURN_SPEED * deltaTime;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
        angle -= TURN_SPEED * deltaTime;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
        float radians = angle * PI / 180.0f;
        position.x += MOVE_SPEED * cos(radians) * deltaTime;
        position.y += MOVE_SPEED * sin(radians) * deltaTime;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
        float radians = angle * PI / 180.0f;
        position.x -= MOVE_SPEED * cos(radians) * deltaTime;
        position.y -= MOVE_SPEED * sin(radians) * deltaTime;
    }
}

void Player::set_player_pose(sf::Vector2f pose){
    position.x = pose.x;
    position.y = pose.y;
}

void Player::set_player_size(float size){
    player_size = size;
}