#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "Map.h"

int main() {
  sf::RenderWindow win(sf::VideoMode(1200,675), "Raycasting");
  Map map(32.0f, 50, 50);

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