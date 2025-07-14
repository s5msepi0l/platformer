#pragma once

/*
    Turns out making 50k gpu calls per frame takes a while so a bunch of this is probably
    gonna have to be refactored to support gpu-side textures, along with animations, textures
    and 90% of the renderer ;(

    alr turns out sdl doesn't suppport gpu batch rendering sooo better hope the scenes are <1000 entities, surfaces & whatever else

*/

#include "renderer.h"
#include "util.h"
#include "game_engine.h"
#include "texture.h"

#include <vector>
#include <ctime>
//#include <set>

#define VISABLE   1
#define INVISIBLE 0

#define RENDER_QUEUE_DEFAULT_QUEUE_SIZE 64

/* *unused*
// mainly used for storing pixel offset but can be used for most other applications as long as the 
// required prescision > 65k
typedef struct {
    u16 x;
    u16 y;
} px_offset;*/


class Render_command {
public:
    uvec16 pos, size;
    f64 rotation;

    i32 z_index;

    // some kind of texture scaling parameter might
    // be needed in the future
};


// should HO
class Render_queue {
public:
    i32 default_queue_size;

    std::unordered_map<std::shared_ptr<Texture>, std::vector<Render_command>> queues;

    void init(i32 d_queue_size = -1) {
        if (d_queue_size < 0)
            default_queue_size = RENDER_QUEUE_DEFAULT_QUEUE_SIZE;
        else default_queue_size = d_queue_size;
    }

    void push(Render_command data, std::shared_ptr<Texture> texture) {
        queues[texture].push_back(data);
    }

    // call before starting to iterate through list
    void sort() {
        for (auto &[texture_id, queue] : queues) {
            std::sort(queue.begin(), queue.end(), [](const Render_command &a, const Render_command &b) {
                return a.z_index < b.z_index; // i got no clue if this sorts it high-low, or low-high
            });
        }
    }

    // call before new frame
    void clear() {
        for (auto &[tex_id, queue] : queues) {
            queue.clear();
            queue.reserve(default_queue_size);
        }
    }
};

class Renderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    Texture_manager *texture_manager;

    u32 width  = 320;
    u32 height = 180;

    i32 tile_size;

    u32 window_width  = 1600;
    u32 window_height = 900;

public:
    Render_queue render_queue;

public:
    void init(u32 w, u32 h, Texture_manager *texture_manager)
    {
        render_queue.init();

        this->texture_manager = texture_manager;

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

    ~Renderer() {
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

    void push_queue(Render_command data, std::shared_ptr<Texture> texture) {
        render_queue.push(data, texture);
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