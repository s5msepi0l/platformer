#ifndef PLATFORMER_GAME_ENGINE
#define PLATFORMER_GAME_ENGINE

#include "renderer.h"
#include <map>

#define LIVE_GAME_OBJECT_LIMIT 64

namespace game_engine {
    f32 Delta_time = 0;

    class component;
    class game_object;

    /* Live objects should be used very sparingly they're 
    collision detection system is quite shit and slow */
    std::map<std::string, game_object*> live_objects;
    
    /* does NOT check collision with other objects
    Static objects update script is executed after
    live objects but are much less computationally expensive */
    std::map<std::string, game_object*> static_objects;
    
    i32 object_count() { return live_objects.size() + static_objects.size(); }

    //manages the amount of time should be slept to keep a consistent framerate
    class frametime_manager {
    private:
        f32 frame_time;

        u64 start_time;
        u64 elapsed_time;

    public:
        inline frametime_manager(u8 Frame_time) {
            frame_time = std::floor(1000.0 / Frame_time);
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

    class component {
        public:
            //used to change the objects position and whatnot
            game_object *self;

            virtual void start() = 0;
            virtual void stop() = 0;
            virtual void update() = 0;
            virtual void collision_enter(game_object *other) = 0;
            virtual void collision_exit(game_object *other) = 0;
    };

    //if size == 0 collision is just not checked
    //more or less just a template for storing game data
    class game_object {
        public:

            /* Will need some refactoring if i want to get some animations up and running */
            buffer_2D<rgb_sprite> model;

            f_vec_2D pos;
            f_vec_2D size;
            
            bool colliding = false;
            game_object *colliding_partner = nullptr;

        private:
            std::string name;
            std::vector<component *> comps;

        public:
            std::string get_name() const { return this->name; }
            void set_name(std::string new_name) { this->name = new_name; }

            bool load_model(std::string path) {
                if ((this->model = readBMP(path)).is_empty())
                    return false;
                
                return false;
            }

            void add_component(component *comp) {
                comp->self = this;
                comp->start();
                comps.push_back(comp);
            }

            inline f32 distance_to(game_object &other) {
                return other.pos.distance_to(this->pos);
            }

            void update_components() {
                for (i32 i = 0; i<comps.size(); i++) {
                    comps[i]->update();
                }
            }

            void set_collision(bool state, game_object *other) {
                std::cout << "SET_COLLISION\n";
                if (this->colliding && !state) {
                    for (i32 i = 0; i<comps.size(); i++) {
                        comps[i]->collision_exit(this->colliding_partner);
                        this->colliding = false;
                    }
                }
                
                if (!this->colliding && state) {
                    for (i32 i = 0; i<comps.size(); i++) {
                        comps[i]->collision_enter(other);
                        this->colliding_partner = other;
                        this->colliding = true;
                    }
                }
                
                if (this->colliding && state) {
                    for (i32 i = 0; i<comps.size(); i++) {
                        comps[i]->collision_exit(this->colliding_partner);
                        comps[i]->collision_enter(other);
                    }
                }   

            }

            ~game_object() {
                std::cout << "destruct\n";
                for (i32 i = 0; i < comps.size(); i++) {
                    comps[i]->stop(); //
                    
                    //delete comps[i];
                }
            }
    };

    class engine {
        public:
            engine(std::string name, u32 width, u32 height, u16 framerate):
            renderer(width, height, name, 1.0),
            frametime(framerate){

            }

            ~engine() { }

        private:
            //runtime variables 
            pipeline_renderer renderer;
            frametime_manager frametime;
            bool running = true;

        public:
            // return true if succesfull
            bool add_object(game_object *object) {
                if (live_objects.size() < LIVE_GAME_OBJECT_LIMIT) {
                    live_objects[object->get_name()] = object;
                    return true;
                }

                return false;
            }

            void add_static_object(game_object *object) {
                static_objects[object->get_name()] = object;
            }

            //return exit code, normal: 0, general error: -1
            u8 run() {
                while(this->running) {
                    frametime.set_start();

                    SDL_Event event;
                    while (SDL_PollEvent(&event) != 0) {
                        if (event.type == SDL_QUIT) {
                            return 0;
                        }
                    }

                    // Game logic
                    for (auto &obj: live_objects) {
                        obj.second->update_components();
                    }

                    for (auto &obj: static_objects) {
                        obj.second->update_components();
                    }

                    collision_detection();
                    //render scene objects
                    
                    for (auto &obj: static_objects) {
                        renderer.pipeline_input(obj.second->model, obj.second->pos);
                    }

                    for (auto &obj: live_objects) {
                        renderer.pipeline_input(obj.second->model, obj.second->pos);
                    }


                    renderer.clear_pipeline();
                    frametime.set_end();
                }

                return 0;
            }

        private:

            //kinda "hackey" and only allows for retangular hitboxes
            void collision_detection() {
                std::cout << "collision_detection\n";
                for (auto &live_obj: live_objects) {
                    for (auto &static_obj: static_objects) {
                        //mind fuck trying to debug                        
                        if (live_obj.second->pos.x < static_obj.second->pos.x + static_obj.second->size.x &&
                            live_obj.second->pos.x + live_obj.second->size.x > static_obj.second->pos.x &&
                            live_obj.second->pos.y < static_obj.second->pos.y + static_obj.second->size.y &&
                            live_obj.second->pos.y + live_obj.second->size.y > static_obj.second->pos.y) {
                            //collision detected            
                            live_obj.second->set_collision(true, static_obj.second);
                        
                            std::cout << "true\n";
                        } else {
                            live_obj.second->set_collision(false, static_obj.second);
                            std::cout << "false\n";
                        }

                    }
                }
            } 
    };
}

#endif
