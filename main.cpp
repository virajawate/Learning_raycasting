#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include "Map.h"
#include "Render.h"
#include "Resources.h"
#include "Player.h"
#include "Editor.h"
float PLAYER_SIZE = 25.0;

int main() {
  float Cell_size = 50.0;
  int Length = 25;
  int Width = 30;

  sf::RenderWindow win(
                        sf::VideoMode({ScreenW, ScreenH}), 
                        "Raycasting",
                        sf::Style::Close | sf::Style::Titlebar
                      );
  sf::Clock Gametime;

  if(ImGui::SFML::Init(win)){
    std::cout<<"ImGUI Initialized.\n";
  } else {
    std::cerr<<"ImGUI Initialization Failed\n";
    return 1;
  }
  // The Maze map grid
  // std::vector<std::vector<int>> Maze = {
  //   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //0
  //   {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //1
  //   {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //2
  //   {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //3
  //   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //4
  //   {1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, //5
  //   {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //6
  //   {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, //7
  //   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //8
  //   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //9
  //   {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //10
  //   {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1}, //11
  //   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  //12
  // };
  
  // Map map(Cell_size, Length, Width); // Backgroud is [num_cellsxLength,num_cellsxWidth]
  // Map maze_map(Cell_size, Maze);

  // Map Color_map(Cell_size, "map/map_1.png");
  Map Color_map{Cell_size};
  Color_map.load("test.map");
  
  if(!Resources::walltextures.loadFromFile("texture/textures.png"))
    std::cerr << "Failed to load wall_textures from textures.png" << std::endl;

  Renderer render;
  render.init();

  Editor editor;
  editor.init(win);

  enum class State { Editor, Game } state = State::Game;

  Player player;
  player.set_player_size(PLAYER_SIZE);
  player.set_player_pose(sf::Vector2f(65,65));

  while(win.isOpen()){
    sf::Time dt = Gametime.restart();
    ImGui::SFML::Update(win, dt);
    while(const std::optional event = win.pollEvent()){
      ImGui::SFML::ProcessEvent(win, *event);
      if(event->is<sf::Event::Closed>()){
        win.close();
      } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
        if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) state = (state == State::Game) ? State::Editor : State::Game;
      }
      if(state == State::Editor) editor.handleEvent(*event);
    }
    ImGui::ShowDemoWindow();
    player.update(dt.asSeconds());
    win.clear();
    if(state == State::Game){
      win.setView(win.getDefaultView());
      render.cast3DNewRayGUI(win, player, Color_map);
    }else if(state == State::Editor){
      Color_map.drawColorGrid(win);
      editor.run(win, Color_map);
      player.draw(win);
    }
    ImGui::SFML::Render(win);
    win.display();
    win.setTitle("Raycaster | " + std::to_string(1.0f / dt.asSeconds()));
  }
  ImGui::SFML::Shutdown();
  return 0;
}