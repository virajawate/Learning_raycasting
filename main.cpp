#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Map.h"

int main() {
  float Cell_size = 50.0;
  int Length = 25;
  int Width = 30;

  sf::RenderWindow win(sf::VideoMode(640,640), "Raycasting");

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
  Map maze_map(Cell_size, Maze);
  while(win.isOpen()){
    sf::Event buffer_event;
    while(win.pollEvent(buffer_event)){
      if(buffer_event.type == sf::Event::Closed){win.close();}
    }
  win.clear();    // Clear the Window
  // map.draw(win);  // Draw the Map
  maze_map.draw(win);  // Draw the Map
  win.display();  // Display in frame
  }

  return 0;
}