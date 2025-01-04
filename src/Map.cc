#include "Map.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

/* Constructor
  1 - Call the constructor with Cell Size, Height Grid and Width Grid
  2 - Call the constructor with Cell Size and the Grid vector
 */
Map::Map(float cell_size, int width, int height) : cellSize(cell_size), grid(height, std::vector(width, 0)){}

Map::Map(float cell_size, std::vector<std::vector<int>> Grid) : cellSize(cell_size), grid(Grid) {}

void Map::draw(sf::RenderTarget &target){
  // vector grid defines the dimensions for the win
  if (grid.empty()){return;}

  // Text Variable Formate
  // Create a backgroud with grid size [Row, Col]
  sf::RectangleShape background(sf::Vector2f(
    static_cast<float>(grid[0].size()) * cellSize,
    static_cast<float>(grid.size()) * cellSize));

  // Set backgroud color to [white]
  background.setFillColor(sf::Color::White);
  target.draw(background);

  // Lets draw individual cells (Smaller than cell size)
  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, 0.95f * cellSize));

  // Draw the Grid with maze ->
  // 0 = Black and 1 = White
  for(size_t r=0; r<grid.size(); r++){
    for(size_t c=0; c<grid[r].size(); c++){
      if(grid[r][c] == 0){cell.setFillColor(sf::Color::White);}
      else if(grid[r][c] == 1){cell.setFillColor(sf::Color::Black);}
      cell.setPosition(c*cellSize, r*cellSize);
      target.draw(cell);
    }
  }
}