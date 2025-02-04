#pragma once

#include "game_engine.h"
#include "util.h"

typedef struct {
    vec2 pos;
    vec2 size;

    i32 z_index;
}Surface;


class Scene {
public:
    virtual void start()  = 0;
    virtual void update() = 0;
    virtual void stop()   = 0;

public:
    /*Add lights, and other stuff here */

    std::unordered_map<u32, std::vector<Surface>>  surfaces;
    std::vector<u32> texture_id;

    void add_surface(Surface surface, u32 texture) {
        if (surfaces.find(texture) == surfaces.end()) {
            texture_id.push_back(texture);
        }

        surfaces[texture].push_back(surface);
    }

    /* doesn't render anything by itself it just pushes the current surfaces to the
    renderers z-buffer
    */
    void render(pipeline_renderer *renderer) {
        for (u32 i = 0; i < texture_id.size(); i++) {
            render_object object;

        }
    }
};

class Scene_manager {
    public:
        void add(std::string name, std::unique_ptr<Scene> scene) {
            scences[name] = std::move(scene);
        }

        Scene *get(std::string name) {
            return scences[name].get();
        }
    
    
    private:
        std::unordered_map<std::string, std::unique_ptr<Scene>> scences;
};