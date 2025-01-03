#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Map.h"

int main() {
  float Num_cells = 25.0;
  int Length = 25;
  int Width = 30;

  sf::RenderWindow win(sf::VideoMode(1366,768), "Raycasting");

  Map map(Num_cells, Length, Width); // Backgroud is [num_cellsxLength,num_cellsxWidth]

  while(win.isOpen()){
    sf::Event buffer_event;
    while(win.pollEvent(buffer_event)){
      if(buffer_event.type == sf::Event::Closed){win.close();}
    }
  win.clear();    // Clear the Window
  map.draw(win);  // Draw the Map
  win.display();  // Display in frame
  }

  return 0;
}