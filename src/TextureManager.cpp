#include "common.hpp"
#include "TextureManager.hpp"

#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
std::shared_ptr<TextureManager> TextureManager::instance = nullptr;
std::shared_ptr<TextureManager> TextureManager::get_instance() {
    if (instance == nullptr)
        instance = std::shared_ptr<TextureManager>(new TextureManager);
    return instance;
}

std::shared_ptr<Texture> TextureManager::get_texture(std::string name) {
    if (instance != nullptr)
        return m_textures[name];
    return nullptr;
}

TextureManager::~TextureManager() {}

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
	"boss_health_bar"
  };

  for (const auto& texture_name : texture_names)
  {
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(); 
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