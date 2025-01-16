#ifndef PLATFORMER_GAME_ENGINE
#define PLATFORMER_GAME_ENGINE

#include "renderer.h"
#include "util.h"
#include <map>

#define DYNAMIC_ENTITIES_LIMIT 64

namespace game_engine {
    class entity;

    class component {
        public:
            //used to change the objects position and whatnot
            entity *self;

            virtual void init() = 0;
            virtual void stop() = 0;
            virtual void tick() = 0;

            virtual void collision_enter(entity *other) = 0;
            virtual void collision_exit(entity *other) = 0;
    };

        class entity {
        public:
            std::vector<component *> comps;


            /* Will need some refactoring if i want to get some animations up and running */
            

            Transform transform;
            
            bool colliding = false;
            entity *colliding_partner = nullptr;

        private:
            std::string name;
        public:
            std::string get_name() const { return this->name; }
            void set_name(std::string new_name) { this->name = new_name; }

            bool load_model(std::string path) {
                
                return true;
            }

            void add_component(component *comp) {
                comp->self = this;
                comp->init();
                comps.push_back(comp);
            }

            inline f32 distance_to(entity &other) {
                return other.transform.pos.distance_to(this->transform.pos);
            }

            void tick_comps() {
                for (i32 i = 0; i<comps.size(); i++) {
                    comps[i]->tick();
                }
            }

            void set_collision(bool state, entity *other) {
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

            ~entity() {
                std::cout << "destruct\n";
                for (i32 i = 0; i < comps.size(); i++) {
                    comps[i]->stop(); //
                    
                    //delete comps[i];
                }
            }
    };

    class ecs {
        public:
            //true if succesfull
            bool add_entity(std::string name, entity *obj) {
                if (dynamic_entities_map.find(name) == dynamic_entities_map.end()) {
                
                    u32 n = obj->comps.size();
                    for (u32 i = 0; i<n; i++) {
                        obj->comps[i]->init();
                    }

                    obj->set_name(name);
                    
                    
                    dynamic_entities_map[name] = obj;
                    dynamic_entities.push_back(obj);
                    return true;
                }

                return false;
            }
    
            entity* get_entity(const std::string& name) {
                return dynamic_entities_map.count(name) ? dynamic_entities_map[name] : nullptr;
            }

            std::vector<entity*>& get_entities() {
               return dynamic_entities;
            }

            //true if succesfull
            bool add_static_entity(const std::string &name, entity *obj) {
                if (static_entities_map.find(name) == static_entities_map.end()) {
                    
                    //call constructor
                    //obj->init();
                    
                    static_entities_map[name] = obj;
                    static_entities.push_back(obj);
                    return true;
                }

                return false;
            }
    
            entity* get_static_entity(const std::string& name) {
                return dynamic_entities_map.count(name) ? dynamic_entities_map[name] : nullptr;
            }

            std::vector<entity*>& get_static_entities() {
               return dynamic_entities;
            }
    
    
            /* does NOT check collision with other objects
            Static objects update script is executed after
            live objects but are much less computationally expensive */
            i32 object_count() { return dynamic_entities.size() + static_entities.size(); }    

        private:
            std::unordered_map<std::string, entity*> static_entities_map;
            std::vector<entity*> static_entities;


            
            std::unordered_map<std::string, entity*> dynamic_entities_map;
            std::vector<entity*> dynamic_entities;
    };

    class Deltatime {
        public:
            Deltatime() : last_time(0.0), delta_time(0.0) {}

            inline const f64 update() {
                f64 current_time = SDL_GetTicks64();
                delta_time = current_time - last_time;
                last_time = current_time;

                return delta_time;
            }


        private:
            f64 last_time;
            f64 delta_time;
    };

    //manages the amount of time should be slept to keep a consistent framerate
    class frametime_manager {
    private:
        f32 frame_time;

        f64 start_time;
        f64 elapsed_time;

    public:
        inline void set_frametime(u32 Frame_time) { this->frame_time = std::floor(1000.0 / Frame_time); }

        inline void set_start() {
            start_time = SDL_GetTicks64();
        }


        // call this function after you've finished frame creating, rendering, keyboard polling etc and are
        // about to restart the loop
        inline void set_end() {
            elapsed_time = SDL_GetTicks64() - start_time;
            if (elapsed_time < frame_time) {
                SDL_Delay(frame_time);
            }
        }
    };

    //if size == 0 collision is just not checked
    //more or less just a template for storing game data

    class engine {
        public:
            ecs entities;
            pipeline_renderer renderer;
            frametime_manager frametime;

            Deltatime delta_time;
            f64 deltatime;

            keyboard_input input;

            bool running = true;

            struct {
                f32 tick_inc;
                f32 ticks;
            }tick;

            u32 framerate;
            u32 tickrate;

            engine(std::string name, u32 width, u32 height, u32 frame_rate, u32 tickrate){
                tick.tick_inc = tickrate / framerate;
            }

            ~engine() { }

        public:
            // return true if succesfull
            bool add_entity(std::string name, entity *object) {
                return entities.add_entity(name, object);
            }

            bool add_static_entity(std::string name, entity *object) {
                return entities.add_static_entity(name, object);
            }

            //return exit code, normal: 0, general error: -1
            u8 run() {
                renderer.start_frame();
                frametime.set_start();

                input.poll();
                if (input.quit) running = false;

                deltatime = delta_time.update();

                tick.ticks += tick.tick_inc;
                if (tick.ticks >= 1.0) {
                    tick_entities();

                    tick.ticks = 0.0;
                }

                renderer.render_frame();
                frametime.set_end();
            }

        private:
            void tick_entities() {
                std::vector<entity*> entity_arr = entities.get_entities();
                for (u32 i = 0; i<entity_arr.size(); i++) {
                    entity_arr[i]->tick_comps();
                }
            }



            //kinda "hackey" and only allows for retangular hitboxes
            void collision_detection() {
                std::cout << "collision_detection\n";
                
                std::vector<entity*> live_objects = entities.get_entities();
                std::vector<entity*> static_objects = entities.get_entities();

                for (auto &dyn_entity: live_objects) {
                    for (auto &static_entity: static_objects) {
                        
                        Transform p1 = dyn_entity->transform;
                        Transform p2 = static_entity->transform;

                        
                        if (AABB(
                            p1.pos.x, p1.pos.y, p1.size.x, p1.size.y,
                            p2.pos.x, p2.pos.y, p2.size.x, p2.size.y
                            )) {
                            
                            //collision detected            
                            dyn_entity->set_collision(true, static_entity);
                        
                        } else {
                            dyn_entity->set_collision(false, static_entity);
                            
                        }

                    }
                }
            } 
    };
}

#endif
