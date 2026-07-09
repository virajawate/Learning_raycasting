#ifndef _MAP_H
#define _MAP_H

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>


class Map{
public:
  //Constructor with grid and cell dimensions
  Map(float cellsize, int width, int height);
  Map(float cellsize, std::vector<std::vector<int>> Grid);
  Map(float cellsize, const std::string &filename);

  // Draw cells
  void draw(sf::RenderTarget& target);
  void drawColorGrid(sf::RenderTarget& target);
  const std::vector<std::vector<int>> getGrid() const;
  const std::vector<std::vector<sf::Color>> getGridColor() const;
  float getCellsize() const;
private:
  // Mapping Grid ROW<COL>
  std::vector<std::vector<int>> grid;
  std::vector<std::vector<sf::Color>> gridColor;
  // Cell Size in pixels
  float cellSize;

};


#endif // _MAP_H