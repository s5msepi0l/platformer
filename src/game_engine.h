#pragma once

#include "renderer.h"
#include "world.h"
#include "util.h"
#include <map>

#define DYNAMIC_ENTITIES_LIMIT 64

namespace game_engine {
    class State;
    class entity;

    class component {
        public:
            //used to interact w the objects position and whatnot
            entity *self;

            virtual void init() = 0;
            virtual void stop() = 0; 
            virtual void tick() = 0;

            virtual void collision_enter(entity *other) = 0;
            virtual void collision_exit(entity *other) = 0;
    };

    class entity {
        public:
            u64 id;

            std::vector<std::unique_ptr<component>> comps;

            /* Will need some refactoring if i want to get some animations up and running */

            Transform transform;
            
            bool collidable;
            bool colliding = false;
            entity *colliding_partner = nullptr;
            State *state;

        private:
            std::string name;

        public:
            std::string get_name() const { return this->name; }
            void set_name(std::string new_name) { this->name = new_name; }

            void add_component(std::unique_ptr<component> comp) {
                comp->self = this;
                comps.push_back(std::move(comp));
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

    /* Get entities nearby, really only used to get collision partners.
     * if in the future there's some performance issues i feel like there
     * could be done some more optimizations relatively easy*/
    class collision_grid {
        public:
            collision_grid(f32 cell_size):cellsize(cell_size) {}

            void clear() {
                grid.clear();
            }

            void add(entity *src) {
                // calculate the grid cells that the entity occupies
                i32 x_start = static_cast<i32>(std::floor(src->transform.pos.x / cellsize));
                i32 y_start = static_cast<i32>(std::floor(src->transform.pos.y / cellsize));

                i32 x_end = static_cast<i32>(std::floor((src->transform.pos.x + src->transform.size.x) / cellsize));
                i32 y_end = static_cast<i32>(std::floor((src->transform.pos.y + src->transform.size.y) / cellsize));

                // add the entity to all the cells it overlaps
                for (i32 x = x_start; x <= x_end; ++x) {
                    for (i32 y = y_start; y <= y_end; ++y) {
                        grid[{(f32)x, (f32)y}].push_back(src);
                    }
                }
            }

            std::vector<entity*> get_neighbors(entity *src) {
                std::vector<entity *> buffer;

                u32 x_start = static_cast<u32>(std::floor(src->transform.pos.x / cellsize));
                u32 y_start = static_cast<u32>(std::floor(src->transform.pos.y / cellsize));

                u32 x_end = static_cast<int>(std::floor((src->transform.pos.x + src->transform.size.x) / cellsize));
                u32 y_end = static_cast<int>(std::floor((src->transform.pos.y + src->transform.size.y) / cellsize));

                for (u32 x = x_start; x <= x_end; ++x) {
                    for (u32 y = y_start; y <= y_end; ++y) {
                        vec2 cell = {(f32)x, (f32)y};
                        
                        if (grid.find(cell) != grid.end()) {
                            for (u32 i = 0; i<grid[cell].size(); i++) {
                                buffer.push_back(grid[cell][i]);
                            }
                        }
                    
                    }
                }

                return buffer;
            }
            
        private:
            f32 cellsize;
            std::unordered_map<vec2, std::vector<entity*> > grid;

            vec2 get_cell(const vec2 pos) {
                return {
                    std::floor(pos.x / cellsize),
                    std::floor(pos.y / cellsize),
                };
            }
    };

    class ecs {
        public:
            State *state;

        public:

            void tick() {
                for (u32 i = 0; i<dynamic_entities.size(); i++) {
                    dynamic_entities[i]->tick_comps();
                }
            }

            const u64 id_idx() { return entity_id_index++; }

            //true if succesfull
            bool add_entity(std::string name, entity *obj, State *state) {
                if (dynamic_entities_map.find(name) == dynamic_entities_map.end()) {
                
                    u32 n = obj->comps.size();
                    for (u32 i = 0; i<n; i++) {
                        obj->comps[i]->init();
                    }

                    obj->set_name(name);
                    obj->id = id_idx();

                    obj->state = state;

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
            bool add_static_entity(const std::string &name, entity *obj, State *state) {
                if (static_entities_map.find(name) == static_entities_map.end()) {
                    
                    u32 n = obj->comps.size();
                    for (u32 i = 0; i<n; i++) {
                        obj->comps[i]->init();
                    }

                    obj->set_name(name);
                    obj->id = id_idx();
                    
                    obj->state = state;

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
            i32 object_count() { return dynamic_entities.size();}    

        private: 
            std::unordered_map<std::string, entity*> static_entities_map;
            std::vector<entity*> static_entities;
            
            std::unordered_map<std::string, entity*> dynamic_entities_map;
            std::vector<entity*> dynamic_entities;

            u64 entity_id_index = 0;
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

    class State {
        public:
            std::unique_ptr<Scene_manager> scene_manager;
            Scene* scene;

            std::unique_ptr<ecs> entities;
            std::unique_ptr<pipeline_renderer> renderer;

            std::unique_ptr<Texture_manager> texture_manager;
            std::unique_ptr<Animation_manager> animation_manager;
        
            std::unique_ptr<frametime_manager> frametime;
            std::unique_ptr<Deltatime> delta_time;
            std::unique_ptr<keyboard_input> input;

            f64 deltatime;

            bool running = true;

            struct {
                f32 tick_inc;
                f32 ticks;
            }tick;

            struct {
                u32 width;
                u32 height;

                u32 framerate;
                u32 tickrate;
            }cfg;

            State() {
                entities =         std::make_unique<ecs>();
                renderer =         std::make_unique<pipeline_renderer>();
                frametime =        std::make_unique<frametime_manager>();

                delta_time =       std::make_unique<Deltatime>();
                input =            std::make_unique<keyboard_input>();

                scene_manager =    std::make_unique<Scene_manager>();
                texture_manager =  std::make_unique<Texture_manager>();
                animation_manager =std::make_unique<Animation_manager>();
            }
    };

    //if size == 0 collision is just not checked
    //more or less just a template for storing game data
    class engine {
        public:
            engine(std::string name, u32 width, u32 height, u32 frame_rate, u32 tickrate):
            state(){
                state.entities->state = &state;

                state.cfg.tickrate = tickrate;
                state.cfg.framerate = frame_rate;

                state.frametime->set_frametime(frame_rate);


                state.tick.tick_inc = (f32)tickrate / state.cfg.framerate;
                std::cout << "tick_inc: " << tickrate / state.cfg.framerate << "\n";

                state.cfg.width =  width;
                state.cfg.height = height;
                state.renderer->init(width, height);
            }

            ~engine() { }

        public:
            State state;

            void render_entities() {
                std::vector<entity*> entities = state.entities->get_entities();

                for (i32 i = 0; i < entities.size(); i++) {
                }
            }

            void render_scene() {
                state.scene->render(state.renderer.get());
            }

            /* sometime maybe add some kind of level loading system via storage,
             * for now though it's just gonna load a static test level till i get
             * things working */
            bool load_level(std::string path, std::unique_ptr<Scene> scene) {
                state.scene_manager->add(path, std::move(scene));

                state.scene = state.scene_manager->get(path);
                
                return true;
            }

            // return true if succesfull
            bool add_entity(std::string name, entity *object) {
                return state.entities->add_entity(name, object, &state);
            }

            bool add_static_entity(std::string name, entity *object) {
                return state.entities->add_static_entity(name, object, &state);
            }

            //return exit code, normal: 0, general error: -1
            u8 run() {
                std::cout << "run()\n";
                state.renderer->start_frame();
                state.frametime->set_start();

                state.deltatime = state.delta_time->update() / state.tick.tick_inc;


                state.input->poll();
                if (state.input->quit){
                    state.running = false;
                }

                state.tick.ticks += state.tick.tick_inc;
                if (state.tick.ticks >= 1.0) {
                    state.entities->tick();

                    state.tick.ticks = 0.0;
                }

                LOG("Check collision between entities");
                collision_detection();

                LOG("depth buffer frame generation\n");                
                state.renderer->z_buffer_frame_gen(state.texture_manager.get(), state.animation_manager.get());

                LOG("Output render");
                state.renderer->render_frame();

                
                state.frametime->set_end(); 

                return 0;
            }

        private:
            void collision_detection() {
                
                std::vector<entity*> live_objects = state.entities->get_entities();
                
                std::vector<entity*> static_objects = state.entities->get_entities();

                /* Check collisions between different entities */
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