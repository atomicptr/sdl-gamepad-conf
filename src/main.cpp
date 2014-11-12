#include <iostream>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <ntd/memory.hpp>

#include <texture_manager.hpp>

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    auto window = ntd::make_smart<SDL_Window>(
        SDL_CreateWindow,
        SDL_DestroyWindow,
        "sdl-gamepad-conf",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600,
        600,
        0
    );

    bool running = true;

    auto renderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);

    auto manager = texture_manager{renderer};

    manager.add("A", "assets/gamepad_A.png");
    manager.add("B", "assets/gamepad_B.png");
    manager.add("X", "assets/gamepad_X.png");
    manager.add("Y", "assets/gamepad_Y.png");
    manager.add("LB", "assets/gamepad_LB.png");
    manager.add("RB", "assets/gamepad_RB.png");
    manager.add("START", "assets/gamepad_START.png");
    manager.add("SELECT", "assets/gamepad_SELECT.png");
    manager.add("LS_X", "assets/gamepad_LS_X.png");
    manager.add("LS_Y", "assets/gamepad_LS_Y.png");
    manager.add("RS_X", "assets/gamepad_RS_X.png");
    manager.add("RS_Y", "assets/gamepad_RS_Y.png");

    auto ptr = manager.get("A");

    SDL_Event event;

    while(running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, ptr, 0, 0);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}
