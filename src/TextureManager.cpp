#include "common.hpp"
#include "TextureManager.hpp"

#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
TextureManager* TextureManager::instance = nullptr;
TextureManager* TextureManager::get_instance() {
    if (instance == nullptr)
        instance = new TextureManager();
    return instance;
}

Texture* TextureManager::get_texture(std::string name) {
    if (instance != nullptr)
        return m_textures[name];
    return nullptr;
}

void TextureManager::load_textures() {
  std::vector<std::string> texture_names{
    "character",
	"boss0",
    "bullet_1",
    "bullet_2",
    "orange_moving_beat",
    "blue_moving_beat",
	"healthbar",
	"enemy0",
	"boss_health_bar",
    "mainmenu",
    "button",
    "song_box",
    "pokemon_songart",
    "blends_songart",
    "marblesoda_songart",
	"enemy1",
	"background2"
  };

  for (const auto& texture_name : texture_names)
  {
    Texture* texture = new Texture(); 
    // TODO: fix the macro
    auto texture_path = textures_path("") + texture_name + ".png";
    std::cout << texture_path << std::endl;
    if (!texture->load_from_file(texture_path.c_str()))
    {
      fprintf(stderr, "Failed to load texture!");
    }
    m_textures[texture_name] = texture;
  }
}