#ifndef PLATFORMER_GAME
#define PLATFORMER_GAME

#include "renderer.h"

enum class keyboard_event{
    null,
    quit,
    forward,
    backward,
    left,
    right
};

//manages the amount of time should be slept to keep a consistent framerate
class frametime_manager {
private:
    u8 frame_time;

    u32 start_time;
    u32 elapsed_time;

public:
    inline frametime_manager(u8 Frame_time = 60) {
        frame_time = static_cast<u8>(std::floor(1000.0 / frame_time));
    }

    inline void set_frametime(u8 Frame_time) { this->frame_time = Frame_time; }

    inline void set_start() {
        start_time = SDL_GetTicks();
        
    }


    // call this function after you've finished frame creating, rendering, keyboard polling etc and are
    // about to restart the loop
    inline void set_end() {
        elapsed_time = SDL_GetTicks() - start_time;
        if (elapsed_time < frame_time) {
            SDL_Delay(frame_time - elapsed_time);
        }
    }

};

class keyboard_input {
    private:
        SDL_Event event;
    public:
        keyboard_event poll() {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) return keyboard_event::quit;
                
                if (event.type == SDL_KEYDOWN) {
                        switch( event.key.keysym.sym ) {
                            case SDLK_UP: 
                                return keyboard_event::forward; 
                            
                            case SDLK_DOWN: 
                                return keyboard_event::backward;
                            
                            case SDLK_LEFT: 
                                return keyboard_event::left;

                            case SDLK_RIGHT: 
                                return keyboard_event::right;

                            default:
                                std::cout << "idfk bro\n";
                                break; 
                        }
                    }   
                }
            return keyboard_event::null;
        } 

};

class game {
    private:
        pipeline_renderer renderer;
        frametime_manager frametime;
        keyboard_input k_input;
        bool running;
        

    public:
        game(i32 xres = 1280, i32 yres = 720, u8 Framerate = 60):
        renderer(xres, yres, "Platformer"),
        frametime(Framerate),
        running(true){
            std::srand(std::time(0));
        }

        //main entry point
            /*    std::srand(time(0));

            pipeline_renderer window(width, height, "Platformer");

            for (i32 i = 0; i<1000 * 1000; i++){
                window.test();
                SDL_Delay(1);
            }

            buffer_2D<rgb_sprite> buffer(20, 20);
            buffer[10][10] = rgb_sprite{rgb{0,0,255},1};
            buffer[12][12] = rgb_sprite{rgb{0,255,0},1};

            f_vec_2D pos(5.0, 5.0);

            for (i32 i = 0; i<10; i++) {
                window.clear_pipeline();
                window.pipeline_input(buffer, pos);
                pos.x += 1.0;            
                window.pipeline_input(buffer, pos);
                
                SDL_Delay(1000);
            }*/
        u8 run() {


            while (this->running) {
                keyboard_event event = k_input.poll();
                frametime.set_start();

                

                frametime.set_end();        
                running = false;
            }

            return 0;
        }

    private:

};

#endif