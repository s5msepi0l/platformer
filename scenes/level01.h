#pragma once

#include "../src/world.h"
#include "../src/game_engine.h"

enum surface_type : u16 {
    plane = 2
};

class level01 : public Scene {
    public:
    game_engine::State *state;
    level01() {

        Texture texture;
        LOG("Initalize random Texture");
        texture.init_random({102, 255, 102}, 50, 50);

        LOG("Apply random texture, texture ID: ", surface_type::plane);
        state->texture_manager->set(texture, surface_type::plane);
        LOG("Applied the thing");

        Surface surface;
        surface.size = {50, 50};
        surface.pos = {25, 50};
        
        LOG("Push level surface");
        this->surfaces[surface_type::plane].push_back(surface);
    }

    void start() {}
    void update() {}
    void stop() {}
};