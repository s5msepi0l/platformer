#include "game.h"

int main(int argc, const char *argv[]) {
    pipeline_renderer renderer(1280, 720, "game", 2);

    for (i32 i = 0; i<1000 * 1000; i++) {
        renderer.test();
        SDL_Delay(125);
    }

    return 0;
}