#include "game.h"

int main(int argc, const char *argv[]) {
    game platformer("Game", 1280, 720, 30);
    platformer.run();

    return 0;
}
