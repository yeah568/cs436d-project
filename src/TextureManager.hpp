#pragma once
#ifndef TextureManager_H
#define TextureManager_H

#include "common.hpp"
#include <unordered_map>

class TextureManager
{
public:
    static std::shared_ptr<TextureManager> get_instance();
    ~TextureManager();
    std::shared_ptr<Texture> get_texture(std::string name);

private:
    TextureManager() {load_textures();};
    void load_textures();
    static std::shared_ptr<TextureManager> instance;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};
#endif