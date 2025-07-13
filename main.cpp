#include "src/game_engine.h"
#include "entities/player.h"
#include "scenes/level01.h"

int main(int argc, const char *argv[]) {
    game_engine::engine game("poop gaming", 1600, 900, 144, 144);

    player p1;
    game.add_entity("Player_1", &p1);

    std::cout << "pre load_level(std::string )\n";
    
    std::unique_ptr<level01> scene = std::make_unique<level01>();
    LOG("Assign state");
    scene->init(&game.state, &game);
    game.load_level("level01", std::move(scene));


    std::cout << "here\n";
    while (game.state.running){
        
        game.run();
        LOG("FPS: ", game.state.frametime->get_fps(), "\n");
    }

    return 0;
}
