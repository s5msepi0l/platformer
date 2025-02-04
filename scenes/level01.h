#pragma once

#include "../src/world.h"

class level01 : public Scene {
    public:
    level01() {
        std::cout << "level01()\n";
        Texture texture;
        texture.init_random({102, 255, 102}, 50, 50);

        texture_manager.set(texture, 0);

        Surface surface;
        surface.texture = 0;
        surface.size = {50, 50};
        surface.pos = {25, 50};
        
        surfaces.push_back(surface);
    }

    void start() {}
    void update() {}
    void stop() {}
};