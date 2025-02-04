#pragma once

#include "util.h"
#include "game_engine.h"
#include "texture.h"

#include <vector>
#include <ctime>

#define VISABLE   1
#define INVISIBLE 0

// mainly used for storing pixel offset but can be used for most other applications as long as the 
// required prescision > 65k
typedef struct {
    u16 x;
    u16 y;
} px_offset;


//object thats passed to the zbuffer 
class render_object {
public:
    uvec16 pos;
    uvec16 size;
};

typedef struct {
    std::unordered_map<u32, std::vector<render_object>> textures;
    std::vector<u32> texture_id;

    void add(u32 texture, render_object object) {
        if (textures.find(texture) == textures.end()) {
            texture_id.push_back(texture);
        }

        textures[texture].push_back(object);
    }
}texture_map;

/* z buffer, uses i32 as a key*/
typedef struct {
    std::unordered_map<i32, texture_map> buffer;
    std::vector<i32> levels;
}Z_buffer;

class pipeline_renderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    u32 width  = 320;
    u32 height = 180;

    i32 tile_size;

    u32 window_width  = 1600;
    u32 window_height = 900;

public:
    Z_buffer z_buffer;

public:
    void init(u32 w, u32 h){
        window_width =  w;
        window_height = h;

        tile_size = window_width / width;
        std::cout << "tile_size: " << tile_size << "\n";
    

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL could not be initialized! SDL_ERROR: %s\n", SDL_GetError());
        }

        this->window = SDL_CreateWindow(
            "poop gaming",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            window_width,
            window_height,
            SDL_WINDOW_SHOWN
        );

        if (nullptr == window) {
            SDL_Log("SDL Window could not be initialized! SDL_Error: %s\n", SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (nullptr == renderer) {
            SDL_Log("SDL renderer could not be initialized! SDL_Error: %s\n", SDL_GetError());   
        }


        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    
    }

    ~pipeline_renderer() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }

    void start_frame() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	
        SDL_RenderClear(renderer);
    }

    void render_frame() {
        SDL_RenderPresent(renderer);
    }

    void z_buffer_push(
        render_object object,
        i32 z_index,
        u32 texture_id
    ) {
        z_buffer.buffer[z_index].add(texture_id, object);

        auto it = std::lower_bound(z_buffer.levels.begin(), z_buffer.levels.end(), z_index, std::greater<i32>());
        if (it == z_buffer.levels.end() || *it != z_index) {
            z_buffer.levels.insert(it, z_index);
        }
    }

    void test() {
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	
        SDL_RenderClear(renderer);

        std::cout << "test\n";
        
        i32 boxsize = 16 / 2;

        for (i32 x = 0; x < width; x++) {
            for (i32 y = 0; y < height; y++) {


                u8 r = rand() % 256;
                u8 g = rand() % 256;
                u8 b = rand() % 256;

                draw_pixel(x, y, 16, g, b);

            }
        }
    }

    void draw_pixel(i32 x, i32 y, u8 r, u8 g, u8 b) {
        SDL_Rect rect{x * tile_size, y * tile_size, tile_size, tile_size};

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
        
    }

    void draw_pixel(i32 x, i32 y, rgba clr) {
        SDL_Rect rect{x * tile_size, y * tile_size, tile_size, tile_size};

        SDL_SetRenderDrawColor(renderer, clr.r, clr.g, clr.b, 255);
        SDL_RenderFillRect(renderer, &rect);
        
    }

    void draw_pixel(i32 x, i32 y, u8 r, u8 g, u8 b, u8 a) {
        SDL_Rect rect{x * tile_size, y * tile_size, tile_size, tile_size};

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
    }
};