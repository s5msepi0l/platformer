#pragma once

#include "util.h"

#include <vector>
#include <ctime>

#define VISABLE   1
#define INVISIBLE 0

typedef struct {
    u8 r = 255;
    u8 g = 255;
    u8 b = 255;
}rgb;

// mainly used for storing pixel offset but can be used for most other applications as long as the 
// required prescision > 65k
typedef struct {
    u16 x;
    u16 y;
} px_offset;

enum class render_type {
    DEFAULT_RGB = 1,
    NULL_RGB = 2 // empty / no render type selected
};

/* render object that's passed to the renderer, add different types of rendering outputs
    like static images or dynamically changing like particles for instance
*/

typedef struct {
    // add more advanced stuff as time goes on

    buf2<rgb> pixels;
}default_rgb;

typedef struct render_obj{
    render_type type = render_type::NULL_RGB;
    void *data;

    void init(render_type t) {
        type = t;

        switch(t) {
            case render_type::DEFAULT_RGB:
                data = new default_rgb;
            break;
        }
    }

    ~render_obj() {
        switch(type) {
            case render_type::DEFAULT_RGB: {
                default_rgb *data_ptr = static_cast<default_rgb*>(data);

                delete data_ptr;
            }

            break;
        }
    }

}render_obj;

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

    void render(render_obj *obj, vec2 offset) {
        switch(obj->type) {
            case render_type::DEFAULT_RGB:
                render_default_rgb(obj->data, offset);

                break;

            default:
                std::cout << "render_obj.type not defined\n";
                return ;
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

    void render_default_rgb(void *src, vec2 offset) {
        default_rgb *data = reinterpret_cast<default_rgb*>(src);
        
        for (int x = 0; x < data->pixels.width; x++) {
            for (int y = 0; y < data->pixels.height; y++) {
                rgb clr = data->pixels[x][y];

                draw_pixel(
                    x + offset.x, y + offset.y,
                    clr.r, clr.g, clr.b);
            }
        }

    }

    void draw_pixel(i32 x, i32 y, u8 r, u8 g, u8 b) {
        SDL_Rect rect{x * tile_size, y * tile_size, tile_size, tile_size};

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
        
    }

    void draw_pixel(i32 x, i32 y, u8 r, u8 g, u8 b, u8 a) {
        SDL_Rect rect{x * tile_size, y * tile_size, tile_size, tile_size};

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
    }
};