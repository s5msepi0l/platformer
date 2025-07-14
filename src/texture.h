#pragma once

#include "util.h"
#include "renderer.h"

buf2<rgba> load_image(std::string path);

buf2<rgba> load_image(std::string path) {
    buf2<rgba> buffer(0, 0);

    i32 w, h, channels;
    u8 *img = stbi_load(path.c_str(), &w, &h, &channels, 0);
    if (img) {
        buffer.reserve(w, h);
        switch(channels) {
            case 3:                
                for (i32 x = 0; x < w; x++) {
                    for (i32 y = 0; y < h; y++) {
                        i32 i = (y * w * x) * channels;

                        u8 r = img[i];
                        u8 g = img[i + 1];
                        u8 b = img[i + 2];
                
                        buffer.set(x, y, {r, g, b, 255});                           
                    }
                }

            case 4:
                for (i32 x = 0; x < w; x++) {
                    for (i32 y = 0; y < h; y++) {
                        i32 i = (y * w * x) * channels;

                        u8 r = img[i];
                        u8 g = img[i + 1];
                        u8 b = img[i + 2];
                        u8 a = img[i + 3];

                        buffer.set(x, y, {r, g, b, a});                           
                    }
                }

                break;
        }


        stbi_image_free(img);
    }

    return buffer;
}

class Texture {
    public:
        SDL_Texture *id;

        i32 pitch; // pixel buffer memory padding size
        i32 row_width;
        
        // Avoid using this directly, use the .get method instead or just copy the texture via the .copy method
        u32 *pixel_buffer; // Does not get changed to null when texture is locked

        bool loaded = false; //Incase i ever feel the need to add texture streaming

        bool locked = false; // is the texture locked e.g Can i access the pixel buffer

        void init(SDL_Renderer *renderer, i32 width, i32 height) {
            id = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING,
                width, height
            );
        }

        void destroy() {
            if (id) {
                SDL_DestroyTexture(id);
                id = nullptr;
            }

            pixel_buffer = nullptr;
            loaded = false;
        }

        ~Texture() {
            destroy();
        }

        inline void lock() {
            locked = true;
            void *pixels;

            SDL_LockTexture(id, NULL, &pixels, &pitch);
            pixel_buffer = (u32*)pixels;
            row_width = pitch / 4;
        }

        inline void unlock() {
            SDL_UnlockTexture(id);
            locked = false;
        }

        // does not check if texture is locked or not
        u32 *get(u32 x, u32 y) {
            return &pixel_buffer[y * row_width + x];
        }

        // If the texture buffer passed isn't the same dimensions as the gpu texture
        // Its gonna start over writing shit and gonna be a massive pain to fix
        void copy(std::shared_ptr<buf2<rgba>> buf) {
            memcpy(pixel_buffer, buf->data.data(), buf->data.size() * sizeof(u32));
        }

        bool operator==(const Texture& other) const {
            return id == other.id;
        }
};

namespace std {
    template<>
    struct hash<Texture> {
        std::size_t operator()(const Texture& tex) const noexcept {
            return std::hash<SDL_Texture*>{}(tex.id);
        }
    };
}

class Texture_manager {
    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    public:
        std::shared_ptr<Texture> get(std::string index) {
            return textures[index];
        }

        void set(std::shared_ptr<Texture> texture, std::string index) {
            textures[index] = texture;
        }
};



/*

class Texture {
    public:
        std::shared_ptr<buf2<rgba>> data;

        Texture() {
            LOG("Initalize Texture buffer");
            data = std::make_shared<buf2<rgba>>();
        }

        // generate random texture based on noise
        void init_random(rgba mask, u32 w, u32 h) {
            data->reserve(w, h);
            for (u32 x = 0; x < w; x++) {
                for (u32 y = 0; y < h; y++) {
                    u32 noise = rand() % 256;
                    
                    rgba *px = data->get(x, y);

                    px->r = (noise * mask.r) / 255;
                    px->g = (noise * mask.g) / 255;
                    px->b = (noise * mask.b) / 255;

                }
            }
        }

};
class Frame {
    public:
        u32 width;
        u32 height;

        buf2<rgba> data;

        bool load(std::string path) {
            data = load_image(path);
            return data.is_empty();
        }
    };

class Animation {
    public:
        std::vector<Frame> frames;
        u32 frame_count;

        u32 framerate;
        u32 frame_index;

        f64 elapsed_time;

        buf2<rgba> get_current_frame() {return frames[frame_index].data;}

        void init(size_t n, u32 fps) {
            frames.reserve(n);
            framerate = fps;

        }

        void load_frames(const std::vector<std::string> &path, u32 w, u32 h) {
            for (u32 i = 0; i < path.size(); i++) {
                load(path[i], w, h);
            }
        }

        void load(std::string path, u32 w, u32 h) {
            Frame frame;
            frame.load(path);

            if (frame.data.width != w || frame.data.height != h) {
                buf2<rgba> img = bilinear_resize(&frame.data, w, h);
                frame.data = img;
            }

            frames.push_back(frame);
        }

        void tick(f64 deltatime) {
            elapsed_time += deltatime;

            if ( elapsed_time >= 1000.0 / framerate) {
       
                
                if (frame_index == frame_count) frame_index = 0;
                else frame_index++;

                elapsed_time -= 1000.0 / framerate;
            }
        }

    };


class Animation_manager{
public:
    std::unordered_map<u32, std::shared_ptr<Animation>> animations;

    std::shared_ptr<Animation> get(u32 index) {
        return animations[index];
    }

    void set(u32 index, Animation animation) {
        animations[index] = std::make_shared<Animation>(animation);
    }
        
};

enum class model_type : u8 {
    TEXTURE,
    ANIMATION
};
    

class Model {
public:
    Transform *src_transform;

    model_type type;

    union {
        u32 texture_id;
        struct {
            u32 id;
            std::shared_ptr<Animation> ptr;
        }animation;
    };

    Model(model_type t) {
        type = t;
    }

    //using any of these methods assumes that the type is ANIMATION
    buf2<rgba> get_current_frame() {
        return animation.ptr->get_current_frame();
    }

    // the path should be a directory of images "dir/x.png"
    //also remember to add a / after the dir name 
    void init_animation(std::string path, std::string filetype, u32 frame_count, u32 client_fps) {
        animation.ptr = std::make_unique<Animation>();
        animation.ptr->init(frame_count, client_fps);

        std::vector<std::string> dir(frame_count);
        for (u32 i = 0; i < frame_count; i++) {
            std::stringstream ss;
            ss << path << i << filetype;

            dir.push_back(ss.str());
        }
            
    }

    void animation_tick(f64 deltatime) {
        animation.ptr->tick(deltatime);
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

buf2<rgba> bilinear_resize(buf2<rgba> *input, u32 new_w, u32 new_h) {
    if (input->is_empty()) {
        throw std::runtime_error("Input buffer is empty dumbass");
    }

    buf2<rgba> output = buf2<rgba>(new_w, new_h); // Preallocate output buffer

    for (u32 y = 0; y < new_h; y++) {
        for (u32 x = 0; x < new_w; x++) {
            float src_x = (x + 0.5f) * input->width / new_w - 0.5f;
            float src_y = (y + 0.5f) * input->height / new_h - 0.5f;

            u32 x0 = static_cast<u32>(src_x);
            u32 y0 = static_cast<u32>(src_y);
            u32 x1 = std::min(x0 + 1, input->width - 1);
            u32 y1 = std::min(y0 + 1, input->height - 1);

            float dx = src_x - x0;
            float dy = src_y - y0;

            rgba *A = input->get(x0, y0);
            rgba *B = input->get(x1, y0);
            rgba *C = input->get(x0, y1);
            rgba *D = input->get(x1, y1);

            rgba final_pixel;
            final_pixel.r = static_cast<u8>((A->r * (1.0f - dx) + B->r * dx) * (1.0f - dy) + (C->r * (1.0f - dx) + D->r * dx) * dy);
            final_pixel.g = static_cast<u8>((A->g * (1.0f - dx) + B->g * dx) * (1.0f - dy) + (C->g * (1.0f - dx) + D->g * dx) * dy);
            final_pixel.b = static_cast<u8>((A->b * (1.0f - dx) + B->b * dx) * (1.0f - dy) + (C->b * (1.0f - dx) + D->b * dx) * dy);
            final_pixel.a = static_cast<u8>((A->a * (1.0f - dx) + B->a * dx) * (1.0f - dy) + (C->a * (1.0f - dx) + D->a * dx) * dy);

            output.set(x, y, final_pixel);
        }
    }

    return output;
}
*/