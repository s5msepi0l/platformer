#pragma once

#include "util.h"
#include "renderer.h"

#define segment(p, q, r) \
    std::min(p.x, r.x) <= q.x && q.x <= std::max(p.x, r.x) && \
    std::min(p.y, r.y) <= q.y && q.y <= std::max(p.y, r.y)

namespace world {
    //magic collision detection function made by chatgpt :P
    float cross_product(ivec2 p1, ivec2 p2, ivec2 p3) {
        return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    }

    int winding_num(ivec2 point, std::vector<ivec2>& polygon) {
        int winding_number = 0;

        for (int i = 0; i < polygon.size(); ++i) {
            ivec2 p1 = polygon[i];
            ivec2 p2 = polygon[(i + 1) % polygon.size()]; // Wrap around to the first point
            
            if (p1.y <= point.y) {
                if (p2.y > point.y) {
                    // Edge crosses the horizontal line above the point
                    if (cross_product(p1, p2, point) > 0) {
                        ++winding_number; // Counterclockwise turn
                    }
                }
            } else {
                if (p2.y <= point.y) {
                    // Edge crosses the horizontal line below the point
                    if (cross_product(p1, p2, point) < 0) {
                        --winding_number; // Clockwise turn
                    }
                }
            }
        }

        return winding_number;
    }

    bool lines_intersect(const ivec2 p1, ivec2 p2, ivec2 q1, ivec2 q2) {
        float d1 = cross_product(q1, q2, p1);
        float d2 = cross_product(q1, q2, p2);
        float d3 = cross_product(p1, p2, q1);
        float d4 = cross_product(p1, p2, q2);

        // General case
        if ((d1 * d2 < 0) && (d3 * d4 < 0)) {
            return true;
        }


        if (d1 == 0 && segment(q1, p1, q2)) return true;
        if (d2 == 0 && segment(q1, p2, q2)) return true;
        if (d3 == 0 && segment(p1, q1, p2)) return true;
        if (d4 == 0 && segment(p1, q2, p2)) return true;

        return false;
    }

    typedef struct {
        std::vector<ivec2> verticies;
    }mesh;


    bool box_collision_mesh(Transform transform, mesh pts) {
        Transform edges[4]{
            transform.pos,
            transform.pos 
        };

        return false;
    }

    // fuck templates
    enum class mesh_type {
        RECTANGLE = 1,  // Transform
        MESH = 2        // slopes, circles and stuff
    };
    
    typedef struct surface_box{
        mesh_type type;
        void *data;

        template <typename T>
        T *get() {
            return static_cast<T*>(data);
        }

        ~surface_box() {
            switch(type) {
                case mesh_type::RECTANGLE:
                    delete get<Transform>();
                    break;

                case mesh_type::MESH:
                    delete get<mesh>();
                    break;
            }
        }

    }surface_box;

    enum class surface_type {
        STATIC,
        DYNAMIC
    };

    class surface {
    public:
        surface_type type;
        surface_box hitmesh;
        render_obj texture;
    };

    class level {
        public:
            std::vector<surface> surfaces;
    };

    class world {
        private:
            std::vector<level> levels;
            level *loaded_level;
    
        public:
            inline void load_level(level src) {
                levels.push_back(src);
            }

            inline void load_level(std::vector<level> src) {
                for (u32 i = 0; i<src.size(); i++) {
                    levels.push_back(src[i]);
                }
            }

            void load(u32 index) {
                loaded_level = &levels[index];
            }

            /* In the future I should probably ad some kind of system so that
             * the engine doesn't check collisions between every surface on the level
             * whether it's even on the screen or not */
            surface *check_collision(Transform transform) {
                for (u32 i = 0; i<loaded_level->surfaces.size(); i++) {
                    switch(loaded_level->surfaces[i].hitmesh.type) {
                        case mesh_type::RECTANGLE: {
                            Transform *rect = loaded_level->surfaces[i].hitmesh.get<Transform>();
                                if AABB(rect->pos.x, rect->pos.y, rect->size.x, rect->size.y,
                                    transform.pos.x, transform.pos.y, transform.size.x, transform.size.y) {                                
                                    return &loaded_level->surfaces[i];
                                }

                            break;
                        }

                            case mesh_type::MESH:
                            /* I'll probabably fully implement this at some point
                             * but I'm gonna focus on getting things up and running first w graphics and stuff
                                if (winding_num(transform.pos, loaded_level->surfaces)) {

                                }
                            */


                                break;
                    }
                }
            
            }
    };
}