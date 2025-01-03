#ifndef _MAP_H
#define _MAP_H

#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>


class Map{
public:
  //Constructor with grid and cell dimensions
  Map(float cellsize, int width, int height);

  // Draw cells
  void draw(sf::RenderTarget& target);
private:
  // ROW<COL>
  std::vector<std::vector<int>> grid; 
  // Cell Size in pixels
  float cellSize;

};


#endif // _MAP_H