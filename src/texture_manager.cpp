#include <texture_manager.hpp>

texture_manager::texture_manager(SDL_Renderer *renderer) : renderer_(renderer) {
}

void texture_manager::add(std::string name, std::string path) {
    auto image = ntd::make_smart<SDL_Surface>(IMG_Load, SDL_FreeSurface, path.c_str());

    auto texture_ptr = SDL_CreateTextureFromSurface(renderer_, image.get());

    textures_.insert(std::make_pair(name, std::shared_ptr<SDL_Texture>(texture_ptr, SDL_DestroyTexture)));
}

SDL_Texture* texture_manager::get(std::string name) {
    return textures_.at(name).get();
}
