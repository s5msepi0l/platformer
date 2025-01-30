#pragma once

#include "game_engine.h"
#include "util.h"

typedef struct {
    u32 texture;

    vec2 pos;
    vec2 size;
}Surface;

class Texture {
    public:
        std::shared_ptr<buf2<rgb>> data;

    void render(
        pipeline_renderer *renderer,
        u32 x_offset, u32 y_offset
    ) {
        for (u32 x = 0; x < data->width; x++) {
            for (u32 y = 0; y < data->height; y++) {
                
                rgb *color = data->get(x, y);

                renderer->draw_pixel(
                    x_offset + x,
                    y_offset + y,
                    *color
                );

            }
        }
    }
};

class Texture_manager {
    private:
        std::unordered_map<u32, Texture> textures;

    public:
        Texture get(u32 index) {
            return textures[index];
        }

        void set(Texture texture, u32 index) {
            textures[index] = texture;
        }
};

class Scene {
public:
    virtual void start()  = 0;
    virtual void update() = 0;
    virtual void stop()   = 0;

public:
    /*Add lights, and other stuff here */
    Texture_manager texture_manager;
    std::vector<Surface> surfaces;

    void render(pipeline_renderer *renderer) {
        for (u32 i = 0; i < surfaces.size(); i++) {
            Texture texture = texture_manager.get(surfaces[i].texture);
            texture.render(
                renderer,surfaces[i].pos.x, surfaces[i].pos.y
            );
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