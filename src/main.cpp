#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <cmath>

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
    std::map<std::string, int> configs;

    auto buttons = std::vector<std::string> {
        "A", "B", "X", "Y", "LB", "RB", "START", "SELECT", "LS", "RS"
    };

    auto hats = std::vector<std::string> {
        "DPADUP", "DPADDOWN", "DPADLEFT", "DPADRIGHT"
    };

    auto axis = std::vector<std::string>{
        "LS_X", "LS_Y", "RS_X", "RS_Y", "LT", "RT"
    };

    for(auto b : buttons) {
        auto str = std::string{"assets/gamepad_"} + b + ".png";

        manager.add(b, str);
    }

    for(auto h : hats) {
        auto str = std::string{"assets/gamepad_"} + h + ".png";

        manager.add(h, str);
    }

    for(auto a : axis) {
        auto str = std::string{"assets/gamepad_"} + a + ".png";

        manager.add(a, str);
    }

    manager.add("NOPE", "assets/gamepad_NOPE.png");

    SDL_Event event;

    int index = 0;
    bool buttons_done = false;
    bool hats_done = false;
    bool joy_moved = false;
    bool hats_are_buttons = false;
    int last_axis = -1;

    auto ptr = manager.get(buttons[index]);

    if(!SDL_WasInit(SDL_INIT_JOYSTICK)) {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }

    auto joynum = SDL_NumJoysticks();

    if(joynum > 0) {
        SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);

        auto name = SDL_JoystickNameForIndex(0);

        std::cout << "Gamepad detected: " << name << std::endl;

        SDL_SetWindowTitle(window.get(), name);
    }

    while(running) {
        joynum = SDL_NumJoysticks();

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }

            if(joynum > 0) {
                if(event.type == SDL_JOYBUTTONDOWN && !buttons_done) {
                    auto button_id = event.jbutton.button;

                    configs.insert(std::make_pair(buttons[index], button_id));

                    index++;

                    if(index == buttons.size()) {
                        buttons_done = true;
                        index = 0;

                        ptr = manager.get(hats[index]);
                    } else {
                        ptr = manager.get(buttons[index]);
                    }
                } else if(event.type == SDL_JOYHATMOTION && event.jhat.value > 0 && !hats_done) {
                    auto hat_id = event.jhat.hat;

                    configs.insert(std::make_pair(hats[index], hat_id));

                    index++;

                    if(index == hats.size()) {
                        hats_done = true;
                        index = 0;

                        ptr = manager.get(axis[index]);
                    } else {
                        ptr = manager.get(hats[index]);
                    }
                } else if(buttons_done && !hats_done && event.type == SDL_JOYBUTTONDOWN) {
                    // hat is button
                    hats_are_buttons = true;

                    auto button_id = event.jbutton.button;

                    configs.insert(std::make_pair(hats[index], button_id));

                    index++;

                    if(index == hats.size()) {
                        hats_done = true;
                        index = 0;

                        ptr = manager.get(axis[index]);
                    } else {
                        ptr = manager.get(hats[index]);
                    }
                }

                auto tval = event.jaxis.value < 0 ? event.jaxis.value * -1 : event.jaxis.value;

                std::cout << "hats done: " << hats_done << " moved? " <<  joy_moved << " motion event? " << (event.type == SDL_JOYAXISMOTION) << " last axis? " << last_axis << " " << int{event.jaxis.axis} << " val " << (tval < 1000) << std::endl;

                if(hats_done && joy_moved && event.type == SDL_JOYAXISMOTION && last_axis == event.jaxis.axis && tval < 1000) {
                    joy_moved = false;
                }

                if(hats_done && !joy_moved && event.type == SDL_JOYAXISMOTION && tval > 32000) {
                    auto axis_id = event.jaxis.axis;

                    configs.insert(std::make_pair(axis[index], axis_id));

                    joy_moved = true;
                    last_axis = int{axis_id};

                    index++;

                    if(index == axis.size()) {
                        running = false;
                    } else {
                        ptr = manager.get(axis[index]);
                    }
                }
            }
        }

        if(joynum == 0) {
            ptr = manager.get("NOPE");
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, ptr, 0, 0);
        SDL_RenderPresent(renderer);
    }

    std::cout << "Hats are buttons? " << hats_are_buttons << std::endl;

    for(auto &c : configs) {
        std::cout << c.first << ": " << c.second << std::endl;
    }

    if(joynum > 0 && SDL_JoystickGetAttached(0)) {
        SDL_JoystickClose(0);
    }

    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}
