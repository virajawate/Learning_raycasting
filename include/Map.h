#ifndef _MAP_H
#define _MAP_H

#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>


class Map{
public:
  Map(float cellsize, int width, int height);
  void draw(sf::RenderTarget& target);
private:
  std::vector<std::vector<int>> grid; // ROW<COL>
  float cellSize;

};


#endif // _MAP_H