#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Map.h"
#include "Render.h"
#include "Player.h"
float PLAYER_SIZE = 25.0;

int main() {
  float Cell_size = 50.0;
  int Length = 25;
  int Width = 30;

  sf::RenderWindow win(
                        sf::VideoMode({ScreenW, ScreenH}), 
                        "Raycasting",
                        sf::Style::Close | sf::Style::Titlebar
                      );
  sf::Clock Gametime;

  // The Maze map grid
  std::vector<std::vector<int>> Maze = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //0
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //1
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //2
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //3
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //4
    {1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, //5
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //6
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //7
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //8
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //9
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //10
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, //11
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  //12
  };

  // Map map(Cell_size, Length, Width); // Backgroud is [num_cellsxLength,num_cellsxWidth]
  // Map maze_map(Cell_size, Maze);
  Map Color_map(Cell_size, "map/map.png");
  Renderer render;
  Player player;
  player.set_player_size(PLAYER_SIZE);
  player.set_player_pose(sf::Vector2f(65,65));

  while(win.isOpen()){
    float deltatime = Gametime.restart().asSeconds();
    while(const std::optional event = win.pollEvent()){
      if(event->is<sf::Event::Closed>()){
        win.close();
      }
    }

    player.update(deltatime);
    
    // Clear the Window
    win.clear();
    
    // Draw the Map
    // map.draw(win);
    
    // Draw the Map
    // maze_map.draw(win);
    
    //Draw the rays
    // render.drawRays(win, player, maze_map);
    
    //Render 3D view
    render.draw3dview(win, player, maze_map);

    //Draw the player
    // player.draw(win);

    // Display in frame
    win.display();
  }

  return 0;
}