#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <ntd/memory.hpp>

#include <texture_manager.hpp>
#include <json/json.h>

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    auto window = ntd::make_smart<SDL_Window>(
        SDL_CreateWindow,
        SDL_DestroyWindow,
        "sdl-gamepad-conf",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600,
        600,
        SDL_WINDOW_OPENGL
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
        "LS_X", "LS_Y", "RS_X", "RS_Y"
    };

    auto triggers = std::vector<std::string>{
        "LT", "RT"
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

    for(auto t : triggers) {
        auto str = std::string{"assets/gamepad_"} + t + ".png";

        manager.add(t, str);
    }

    manager.add("NOPE", "assets/gamepad_NOPE.png");

    SDL_Event event;

    int index = 0;

    bool buttons_done = false;
    bool hats_done = false;
    bool axis_done = false;

    bool joy_moved = false;
    bool hats_are_buttons = false;
    int last_axis = -1;

    auto ptr = manager.get(buttons[index]);

    auto joynum = SDL_NumJoysticks();

    if(joynum > 0) {
        SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);

        auto name = SDL_JoystickNameForIndex(0);

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

                if(hats_done && !axis_done) {
                    auto value = event.jaxis.value < 0 ? event.jaxis.value * -1 : event.jaxis.value;

                    if(joy_moved && event.type == SDL_JOYAXISMOTION && last_axis == event.jaxis.axis && value < 1000) {
                        joy_moved = false;
                    }

                    if(!joy_moved && event.type == SDL_JOYAXISMOTION && value > 28000) {
                        auto axis_id = event.jaxis.axis;

                        configs.insert(std::make_pair(axis[index], axis_id));

                        joy_moved = true;
                        last_axis = int{axis_id};

                        index++;

                        if(index == axis.size()) {
                            axis_done = true;
                            index = 0;
                            joy_moved = false;
                            last_axis = -1;

                            ptr = manager.get(triggers[index]);
                        } else {
                            ptr = manager.get(axis[index]);
                        }
                    }
                }

                if(axis_done) {
                    if(joy_moved && event.type == SDL_JOYAXISMOTION && last_axis == event.jaxis.axis && event.jaxis.value < -30000) {
                        joy_moved = false;
                    }

                    if(event.type == SDL_JOYAXISMOTION && event.jaxis.value > 30000) {
                        auto trigger_id = event.jaxis.axis;

                        configs.insert(std::make_pair(triggers[index], trigger_id));

                        joy_moved = true;
                        last_axis = int{trigger_id};

                        index++;

                        if(index == triggers.size()) {
                            running = false;
                        } else {
                            ptr = manager.get(triggers[index]);
                        }
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

    Json::Value root;
    Json::Value config;
    root["hats_are_buttons"] = hats_are_buttons;

    for(auto &c : configs) {
        config[c.first.c_str()] = c.second;
    }

    root["config"] = config;

    std::cout << root.toStyledString() << std::endl;

    if(joynum > 0 && SDL_JoystickGetAttached(0)) {
        SDL_JoystickClose(0);
    }

    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    return 0;
}
