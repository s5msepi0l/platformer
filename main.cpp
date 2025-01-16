#include "src/game_engine.h"

int main(int argc, const char *argv[]) {
    game_engine::engine game("poop gaming", 1600, 900, 60, 60);


    while (game.running) game.run();

    return 0;
}
