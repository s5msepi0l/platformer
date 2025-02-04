#include "src/game_engine.h"
#include "entities/player.h"
#include "scenes/level01.h"

int main(int argc, const char *argv[]) {
    game_engine::engine game("poop gaming", 1600, 900, 144, 144);

    player p1;
    game.add_entity("Player_1", &p1);

    std::cout << "pre load_level(std::string )\n";
    
    std::unique_ptr<level01> scene = std::make_unique<level01>();

    game.load_level("level01", std::move(scene));


    std::cout << "here\n";
    while (game_engine::state::state.running){
        
        game.run();
    }

    return 0;
}
