#ifndef _MAP_H
#define _MAP_H

#include <cstddef>
#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <cstddef>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Resources.h"

typedef std::vector<std::vector<int>>  MapGrid;
typedef std::vector<std::vector<sf::Color>>  MapColorGrid;
typedef std::vector<std::vector<sf::Sprite>> MapTextureGrid;

class Map{
public:
  //Constructor with grid and cell dimensions
  Map(float cellsize);
  Map(float cellsize, int width, int height);
  Map(float cellsize, std::vector<std::vector<int>> Grid);
  Map(float cellsize, const std::string &filename);
  void SetMap(int x, int y, sf::Color values);
  void save(const std::filesystem::path &path);
  void saveImage(const std::string& filename);
  void load(const std::filesystem::path &path);

  // Draw cells
  void draw(sf::RenderTarget& target); // NOT USED
  void drawColorGrid(sf::RenderTarget& target); // NOT USED
  void drawColorGridTexture(sf::RenderTarget& target);
  const std::vector<std::vector<int>> getGrid() const;
  const std::vector<std::vector<sf::Color>> getGridColor() const;
  float getCellsize() const;
private:
  // Mapping Grid ROW<COL>
  MapGrid grid;
  MapColorGrid gridColor;
  // Cell Size in pixels
  float cellSize;

};


#endif // _MAP_H