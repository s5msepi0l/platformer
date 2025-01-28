#include "src/game_engine.h"
#include "entities/player.h"

int main(int argc, const char *argv[]) {
    game_engine::engine game("poop gaming", 1600, 900, 144, 144);

    player p1;
    game.add_entity("Player_1", &p1);

    while (game_engine::state::state.running){
        
        game.run();
    }

    return 0;
}
