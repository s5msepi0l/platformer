#pragma once

#include "../src/world.h"
#include "../src/game_engine.h"

enum surface_type : u16 {
    plane = 2
};

class level01 : public Scene {
    public:
    game_engine::engine *engine;
    game_engine::State *state;
    void init(game_engine::State *State, game_engine::engine *Engine) {
        this->state = State;
        this->engine = Engine;

        Texture texture;
        LOG("Initalize random Texture");
        texture.init_random({102, 255, 255}, 50, 50);

        LOG("Apply random texture, texture ID: ", surface_type::plane);
        state->texture_manager->set(texture, surface_type::plane);
        LOG("Applied the thing");

        Surface surface;
        surface.size = {50, 50};
        surface.pos = {100, 100};
        
        LOG("Push level surface");

        
        this->add_surface(surface, surface_type::plane);
    }

    void start() {}
    void update() {}
    void stop() {}
};