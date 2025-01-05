#include "Player.h"
#include <iostream>
#include <math.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
constexpr float PI = M_PI;
constexpr float TURN_SPEED = 50.0f;
constexpr float MOVE_SPEED = 50.0f;

std::vector<float> Player::get_player_pose(){
    /* Player Pose
        Pose -> X
        Pose -> Y
        Pose -> Orientation
    */

    std::vector<float> player_pose;
    player_pose[0] = position.x;
    player_pose[1] = position.y;
    player_pose.push_back(angle);
    return player_pose;
}

void Player::draw(sf::RenderTarget &target){
    sf::CircleShape circle(5.0f);
    circle.setPosition(position);
    circle.setFillColor(sf::Color::Blue);
    sf::RectangleShape line(sf::Vector2f(150.0f, 150.0f));
    line.setPosition(position);
    line.setRotation(angle);
    line.setFillColor(sf::Color::Green);
    target.draw(line);
    target.draw(circle);
}

void Player::update(float deltaTime){
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        angle += TURN_SPEED * deltaTime;
        // std::cout<<angle<<std::endl;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        angle -= TURN_SPEED * deltaTime;
        // std::cout<<angle<<std::endl;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        float radians = angle * PI / 180.0f;
        position.x += MOVE_SPEED * cos(radians) * deltaTime;
        position.y += MOVE_SPEED * sin(radians) * deltaTime;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        float radians = angle * PI / 180.0f;
        position.x -= MOVE_SPEED * cos(radians) * deltaTime;
        position.y -= MOVE_SPEED * sin(radians) * deltaTime;
    }
}

void Player::set_player_pose(sf::Vector2f pose){
    position.x = pose.x;
    position.y = pose.y;
}