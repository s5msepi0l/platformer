#ifndef PLATFORMER_RENDERER
#define PLATFORMER_RENDERER

#include "util.h"

#include <vector>
#include <ctime>
#include <SDL2/SDL.h>

#define VISABLE   1
#define INVISIBLE 0

typedef struct {
    u8 r = 255;
    u8 g = 255;
    u8 b = 255;
}rgb;


// pixel is invisible unless specifically specified
typedef struct rgb_sprite {
    rgb clr;
    u8 state = 0;
} rgb_sprite;


// mainly used for storing pixel offset but can be used for most other applications as long as the 
// required prescision > 65k
typedef struct {
    u16 x;
    u16 y;
} px_offset;

/* object that is generally used to handle "larger" 2d rgb data chunks be it 
 * visible player space or or a cached level space which is fed to the render pipeline 
 * I'm hoping that designing each element to only have a rgb value won't bite me in the ass later
 * automatically allocated to heap
*/
template <typename T>
class buffer_2D {
private:
    std::vector<T> data;

public:
    int width;
    int height;

    //preallocate memory to it (x * y * sizeof(rgb))
    buffer_2D(int w, int h):
    data(w * h),
    width(w),
    height(h)
    {    
    }

    // get direct data pointer
    std::vector<T> ref() const { return this->data; }

    T *operator[](int index) {
        std::cout << "index:" << index << '\n';
        return &this->data[index * width];
    }

    const T *operator[](int index) const {
        std::cout << "index:" << index << '\n';
        return &data[index * width];
    }

};

class pipeline_renderer {
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    i32 width;
    i32 height;

    i32 w_tile_n;
    i32 h_tile_n;

    i32 tile_scale; // multiplicator
    static constexpr i32 init_tile_scale = 8;
    

public:
    pipeline_renderer(i32 Width, i32 Weight, std::string name, f32 Tile_scale = 1):
    width(Width),
    height(Weight),
    tile_scale(Tile_scale * init_tile_scale)
    {

        h_tile_n = height / tile_scale;
        std::cout << "h_tile_n" << h_tile_n << '\n';
        w_tile_n = width / tile_scale;


        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL could not be initialized! SDL_ERROR: %s\n", SDL_GetError());
        }

        this->window = SDL_CreateWindow(
            name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_SHOWN
        );

        if (nullptr == window) {
            SDL_Log("SDL Window could not be initialized! SDL_Error: %s\n", SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (nullptr == renderer) {
            SDL_Log("SDL renderer could not be initialized! SDL_Error: %s\n", SDL_GetError());   
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    
    }

    ~pipeline_renderer() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }

    void test() {
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	
        SDL_RenderClear(renderer);

        std::cout << "test\n";
        
        i32 boxsize = 16 / 2;

        for (i32 i = 0; i<w_tile_n; i++) {
            for (i32 j = 0; j<h_tile_n; j++) {
                SDL_Rect rect;
                rect.x = i * tile_scale;
                rect.y = j * tile_scale;
                rect.w = tile_scale;
                rect.h = tile_scale;

                u8 r = rand() % 256;
                u8 g = rand() % 256;
                u8 b = rand() % 256;

                SDL_SetRenderDrawColor(renderer, r, 64, b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
        }
    }


    // use this only when you need to render a sprite
    void pipeline_input(buffer_2D<rgb_sprite> &obj, f_vec_2D offset) {
        for (i32 i = 0; i<obj.width; i++) {
            for (i32 j = 0; j<obj.height; j++) {
                if (obj[i][j].state == VISABLE) {
                    std::cout << "DRAWING\n";
                    SDL_Rect rect;
                    std::cout << "offset x: " << (offset.x * tile_scale) << '\n';
                    std::cout << "offset y: " << (offset.y * tile_scale) << '\n';
                    rect.x = (i * tile_scale) + (offset.x * tile_scale);
                    rect.y = (j * tile_scale) + (offset.y * tile_scale);
                    rect.w = tile_scale;
                    rect.h = tile_scale;

                    rgb clr = obj[i][j].clr;

                    SDL_SetRenderDrawColor(
                        renderer,
                        clr.r,
                        clr.g,
                        clr.b,
                        255
                    );

                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }

    void pipeline_input(buffer_2D<rgb> obj) {

    }

    inline void render() const {
        SDL_RenderPresent(renderer);
    }

    // call this when restarting the frame processing process
    // it just fills the internal sdl render pipeline with a solid black color
    inline void clear_pipeline() {
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	
        SDL_RenderClear(renderer);
    }

private:
};


#endif