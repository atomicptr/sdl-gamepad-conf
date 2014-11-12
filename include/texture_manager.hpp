#ifndef __TEXTURE_MANAGER_HPP__
#define __TEXTURE_MANAGER_HPP__

#include <unordered_map>
#include <memory>
#include <utility>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <ntd/memory.hpp>

class texture_manager {
public:
    texture_manager(SDL_Renderer*);

    void add(std::string, std::string);
    SDL_Texture* get(std::string);

private:
    std::unordered_map<std::string, std::shared_ptr<SDL_Texture>> textures_;

    SDL_Renderer* renderer_;
};

#endif
