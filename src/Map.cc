#include "Map.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

/* Constructor


 */
Map::Map(float cellsize, int width, int height) : cellSize(cellsize), grid(height, std::vector(width, 0)){}

Map::Map(float cellsize, std::vector<std::vector<int>> Grid) : cellSize(cellsize), grid(Grid) {}

void Map::draw(sf::RenderTarget &target){
  // vector grid defines the dimensions for the win
  if (grid.empty()){return;}

  // Text Variable Formate
  // Create a backgroud with grid size [Row, Col]
  sf::RectangleShape background(sf::Vector2f(
    static_cast<float>(grid[0].size()) * cellSize,
    static_cast<float>(grid.size()) * cellSize));

  // Set backgroud color to [white]
  background.setFillColor(sf::Color::Red);
  target.draw(background);

  // Lets draw individual cells (Smaller than cell size)
  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, 0.95f * cellSize));

  for(size_t r=0; r<grid.size(); r++){
    for(size_t c=0; c<grid[r].size(); c++){

      if(grid[r][c] == 0){cell.setFillColor(sf::Color::Black);}
      else if(grid[r][c] == 1){cell.setFillColor(sf::Color::White);}
      cell.setPosition(c*cellSize, r*cellSize);
      target.draw(cell);
    }
  }
}