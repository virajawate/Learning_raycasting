#include "Map.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

/* Constructor
1 - Call the constructor with Cell Size, Height Grid and Width Grid
2 - Call the constructor with Cell Size and the Grid vector
*/
Map::Map(float cell_size, int width, int height) : cellSize(cell_size), grid(height, std::vector(width, 0)){}

Map::Map(float cell_size) : cellSize(cell_size), grid() {}
Map::Map(float cell_size, MapGrid Grid) : cellSize(cell_size), grid(Grid) {}

Map::Map(float cell_size, const std::string &filename) : cellSize(cell_size) {
  sf::Image img;
  if(!img.loadFromFile(filename)){
    std::cerr << "Failed to load map image" << std::endl;
    return;
  } else {
    std::cout << "Got the map" << std::endl;
  }

  gridColor = std::vector(img.getSize().y, std::vector(img.getSize().x, sf::Color::Black));

  for(unsigned y=0; y<img.getSize().y; y++){
    for(unsigned x=0; x<img.getSize().x; x++){
      gridColor[y][x] = img.getPixel({x, y});
    }
  }
}

// NOT USED 
void Map::draw(sf::RenderTarget &target){
  // vector grid defines the dimensions for the win
  if (grid.empty()){return;}
  // Text Variable Formate
  // Create a backgroud with grid size [Row, Col]
  sf::RectangleShape background(sf::Vector2f(
    static_cast<float>(grid[0].size()) * cellSize,
    static_cast<float>(grid.size()) * cellSize)
  );
    
  // Set backgroud color to [white]
  background.setFillColor(sf::Color::White);
  target.draw(background);

  // Lets draw individual cells (Smaller than cell size)
  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, 0.95f * cellSize));

  // Draw the Grid with maze ->
  // 0 = Black and 1 = White
  for(size_t r=0; r<grid.size(); r++){
    for(size_t c=0; c<grid[r].size(); c++){
      if(grid[r][c] == 0){cell.setFillColor(sf::Color(150,150,150));}
      else if(grid[r][c] == 1){cell.setFillColor(sf::Color::Black);}
      cell.setPosition(sf::Vector2f{static_cast<float>(c*cellSize), static_cast<float>(r*cellSize)});
      target.draw(cell);
    }
  }
}

// NOT USED
void Map::drawColorGrid(sf::RenderTarget& target){
  if(gridColor.empty()){
    return;
  }

  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, cellSize * 0.95));

  for(size_t y = 0; y < gridColor.size(); y++){
    for(size_t x = 0; x < gridColor[y].size(); x++){
      // cell.setFillColor(gridColor[y][x]);
      if(gridColor[y][x] == sf::Color::Black){
        cell.setFillColor(sf::Color::Black);
      } else {
        cell.setFillColor(sf::Color::White);
      }
      cell.setPosition(sf::Vector2f(x,y)*cellSize + sf::Vector2f(0.025f*cellSize,0.025f*cellSize));
      target.draw(cell);
    }
  }
}

void Map::drawColorGridTexture(sf::RenderTarget& target){
  if(gridColor.empty()){
    return;
  }
  int textureSize = Resources::walltextures.getSize().y;
  int textureNo = 0;
  for(size_t y = 0; y < gridColor.size(); y++){
    for(size_t x = 0; x < gridColor[y].size(); x++){
      /** Put Texture in Grid */
      if(gridColor[y][x] == sf::Color::Black){
        continue;
      } else if(gridColor[y][x] == sf::Color::Cyan){
        textureNo = 1;
      } else if(gridColor[y][x] == sf::Color::Red){
        textureNo = 2;
      } else if(gridColor[y][x] == sf::Color::Green){
        textureNo = 3;
      } else if(gridColor[y][x] == sf::Color::Yellow){
        textureNo = 4;
      }
      sf::Sprite wall{
        Resources::walltextures,
        sf::IntRect{
          {textureNo * textureSize, 0},
          {textureSize, textureSize}
        }
      };
      wall.setPosition({
        static_cast<float>(x) * cellSize,
        static_cast<float>(y) * cellSize
      });
      wall.setScale({
        cellSize / static_cast<float>(textureSize),
        cellSize / static_cast<float>(textureSize)
      });
      target.draw(wall);
    }
  }
}


const std::vector<std::vector<int>> Map::getGrid() const { return grid; }
const std::vector<std::vector<sf::Color>> Map::getGridColor() const { return gridColor; }

float Map::getCellsize() const { return cellSize; }

void Map::load(const std::filesystem::path &path){
  std::ifstream in{path, std::ios::in | std::ios::binary};
  if(!in.is_open()){
    std::cerr<<"Failed to open file \"" << path << "\" for output.\n"; 
  }
  size_t w, h;
  in.read(reinterpret_cast<char *>(&w), sizeof(w));
  in.read(reinterpret_cast<char *>(&h), sizeof(h));

  gridColor = std::vector(h, std::vector<sf::Color>(w, sf::Color::Black));
  for(size_t y=0; y < gridColor.size(); y++){
    for(size_t x=0; x < gridColor[y].size(); x++){
      in.read(reinterpret_cast<char *>(&gridColor[y][x]), sizeof(gridColor[y][x]));
    }
  }
}

void Map::save(const std::filesystem::path &path){  
  std::ofstream out{path, std::ios::out | std::ios::binary};
  if(!out.is_open()){
    std::cerr<<"Failed to open file \"" << path << "\" for output.\n"; 
  }
  if (gridColor.empty()){
    return;
  }

  size_t w = gridColor.size(); 
  size_t h = gridColor[0].size();
  out.write(reinterpret_cast<const char *>(&w), sizeof(w));
  out.write(reinterpret_cast<const char *>(&h), sizeof(h));

  for(size_t y=0; y < gridColor.size(); y++){
    for(size_t x=0; x < gridColor[y].size(); x++){
      out.write(reinterpret_cast<const char *>(&gridColor[y][x]), sizeof(gridColor[y][x]));
    }
  }
  std::cout<<"Map Saved"<<std::endl;
}

void Map::SetMap(int x, int y, sf::Color values){
  if(y > 0 && y < gridColor.size() && x > 0 && x < gridColor[y].size()){
    gridColor[y][x] = values; 
  }
}