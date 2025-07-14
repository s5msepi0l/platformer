#pragma once

#include "game_engine.h"
#include "util.h"

typedef struct {
    vec2 pos, size;

    f64 rotation;
    i32 z_index;

    std::shared_ptr<Texture> texture;

    //Add other stuff here if needed like material or whatever

}Surface;

class Scene {
public:
    virtual void start()  = 0;
    virtual void update() = 0;
    virtual void stop()   = 0;

public:
    Scene(game_engine::State *state, f32 spatial_grid_size = 32.0): // just a wild guess of what
    // works but should probably be changed according to needs
    surfaces(32.0),
    state(state) {

    }

    /*Add lights, and other stuff here */

    game_engine::spatial_grid<Surface> surfaces;

    /* 
    */
    void render(Renderer *renderer) {
        LOG("Render scene\n");

        for (const auto& pair : surfaces.grid) {
            const std::vector<Surface*> &surface_grid = pair.second;
            for (size_t i = 0; i < surface_grid.size(); ++i) {
                Surface *surface = surface_grid[i];

                // pass surface parameters to renderer

                /* scene manager requests texture X -> from texture manager
                    if Texture X is not in cache send request via queue and return
                    Placeholder Texture -> pack level data into render_command
                    and add

                    Probably slow asf but hey Im expecting ts to be able make up to 1000+
                    gpu calls per frame so i doubt this is gonna get bottlenecked
                    by copying a few pointers
                */
            }
        }

    }

private:
    game_engine::State *state;
    Renderer *renderer;
};

class Scene_manager {
    public:
        void add(std::string name, std::unique_ptr<Scene> scene) {
            scences[name] = std::move(scene);
        }

        Scene *get(std::string name) {
            return scences[name].get();
        }
    
    
    public:
        std::unordered_map<std::string, std::unique_ptr<Scene>> scences;
};