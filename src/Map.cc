#include "Map.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>


Map::Map(float cellsize, int width, int height) : cellSize(cellsize), grid(height, std::vector(width, 0)){}

void Map::draw(sf::RenderTarget &target){
  if (grid.empty()){return;}

  sf::RectangleShape background(sf::Vector2f(
    static_cast<float>(grid[0].size()) * cellSize,
    static_cast<float>(grid.size()) * cellSize));
  background.setFillColor(sf::Color::White);
  target.draw(background);

  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, 0.95f * cellSize));
  cell.setFillColor(sf::Color::Blue);

  for(size_t r=0; r<grid.size(); r++){
    for(size_t c=0; c<grid[r].size(); c++){
      if(grid[r][c] == 1){
        cell.setPosition(c*cellSize, r*cellSize);
        target.draw(cell);
      }
    }
  }
  
}