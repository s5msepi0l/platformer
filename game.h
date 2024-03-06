#ifndef PLATFORMER_GAME
#define PLATFORMER_GAME

#include "game_engine.h"

using namespace game_engine;

class Player1: public component {
    private:
        keyboard_input input;
        f32 speed = 4;

    public:
        void stop() override {}
        void collision_enter(game_object *other) override {
            std::cout << "enter\n";
        }
        void collision_exit(game_object *other) override {
            std::cout << "\n\n\n__EXIT__\n\n\n";

            auto *pos = &other->pos;
            pos->x = (self->pos.x + pos->x/2) / 2;
            pos->y = (self->pos.y + pos->y/2) / 2;
        }


        void start() override {
            std::cout << "start\n";

        }

        void update() override {
            f_vec_2D event = input.poll();
            self->pos += (event * speed);
        }
};

class Player2: public component {

    public:
        void stop() override {}
        void collision_enter(game_object *other) override {}
        void collision_exit(game_object *other) override {}


        void start() override {
            std::cout << "start2\n";
        }

        void update() override {}
};


class game {
private:
    engine Engine;

public:
    game(std::string name, u16 width, u16 height, u8 framerate):
    Engine(name, width, height, framerate) {
    }

    void run() {
        Player1 script1;
        game_object p1;
        p1.add_component(&script1);
        
        p1.size = f_vec_2D(14, 14);
        p1.load_model("./img/char/base1.bmp");

        if (Engine.add_object(&p1)) {
            std::cout << "added object\n";
        }

        Player2 script2;
        game_object p2;
        p2.add_component(&script2);
        
        p2.size = f_vec_2D(2, 2);
        p2.load_model("./img/char/base.bmp");

        Engine.add_static_object(&p2);
            std::cout << "added object\n";

        Engine.run();
    }
};

#endif
