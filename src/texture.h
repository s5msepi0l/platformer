#pragma once

#include "util.h"
#include "renderer.h"

class Texture {
    public:
        std::shared_ptr<buf2<rgba>> data;

        Texture() {
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

        /*void render(
            pipeline_renderer *renderer,
            u32 x_offset, u32 y_offset
        ) {
            std::cout << "render_world()\n";

            for (u32 x = 0; x < data->width; x++) {
                for (u32 y = 0; y < data->height; y++) {
                    
                    rgba *color = data->get(x, y);

                    renderer->draw_pixel(
                        x_offset + x,
                        y_offset + y,
                        *color
                    );

                }
            }
        }*/
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

buf2<rgba> bilinear_resize(buf2<rgba>input, u32 new_w, u32 new_h) {
    if (input.is_empty()) {
        throw std::runtime_error("Input buffer is empty dumbass");
    }

    buf2<rgba> output = buf2<rgba>(new_w, new_h); // Preallocate output buffer

    for (u32 y = 0; y < new_h; y++) {
        for (u32 x = 0; x < new_w; x++) {
            float src_x = (x + 0.5f) * input.width / new_w - 0.5f;
            float src_y = (y + 0.5f) * input.height / new_h - 0.5f;

            u32 x0 = static_cast<u32>(src_x);
            u32 y0 = static_cast<u32>(src_y);
            u32 x1 = std::min(x0 + 1, input.width - 1);
            u32 y1 = std::min(y0 + 1, input.height - 1);

            float dx = src_x - x0;
            float dy = src_y - y0;

            rgba *A = input.get(x0, y0);
            rgba *B = input.get(x1, y0);
            rgba *C = input.get(x0, y1);
            rgba *D = input.get(x1, y1);

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