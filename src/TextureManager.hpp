#pragma once
#ifndef TextureManager_H
#define TextureManager_H

#include "common.hpp"
#include <unordered_map>

class TextureManager
{
public:
    static TextureManager* get_instance();
    ~TextureManager();
    Texture* get_texture(std::string name);

private:
    TextureManager() {load_textures();};
    void load_textures();
    static TextureManager* instance;
    std::unordered_map<std::string, Texture*> m_textures;
};
//TextureManager* TextureManager::instance = nullptr;
#endif