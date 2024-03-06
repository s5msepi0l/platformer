#ifndef PLATFORMER_SPRITE
#define PLATFORMER_SPRITE

#include "util.h"
#include <fstream>

#pragma pack(push, 1) // remove struct padding
typedef struct {
    u16 signature;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 data_offset;
} BMP_header;

typedef struct {
    u32 header_size;
    i32 width;
    i32 height;
    u16 color_planes;
    u16 bits_per_pixel;
    u32 compression_method;
    u32 image_size;
    i32 horizontal_resolution;
    i32 vertical_resolution;
    u32 colors_in_palette;
    u32 important_colors;
} DIB_header;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a; // transparency (0 = fully transparent, 255 = fully opaque)
} rgb_sprite;
#pragma pack(pop) 

typedef union {
    buffer_2D<rgb_sprite> buf;
    std::nullptr_t null;
} bmp_file;


//slow af results should probably be cached
buffer_2D<rgb_sprite> readBMP(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return buffer_2D<rgb_sprite>();
    }

    BMP_header bmp_header;
    file.read(reinterpret_cast<char*>(&bmp_header), sizeof(BMP_header));

    if (bmp_header.signature != 0x4D42) {
        std::cerr << "Not a valid BMP file." << std::endl;
        return buffer_2D<rgb_sprite>();
    }

    DIB_header dib_header;
    file.read(reinterpret_cast<char*>(&dib_header), sizeof(DIB_header));

    if (dib_header.bits_per_pixel != 32) {
        std::cerr << "Unsupported BMP format. This parser only supports 32-bit BMP without compression." << std::endl;
        return buffer_2D<rgb_sprite>();
    }

    // move file cursor to the start of pixel data
    file.seekg(bmp_header.data_offset, std::ios::beg);

    // calculate row size, including padding
    size_t row_size = ((dib_header.bits_per_pixel * dib_header.width + 31) / 32) * 4;

    // read pixel data
    buffer_2D<rgb_sprite> pixel_data(dib_header.width, dib_header.width);
    //std::vector<std::vector<pixel>> pixel_data(dib_header.height, std::vector<pixel>(dib_header.width));
    //int i = dib_header.height - 1; i >= 0; --i) {
    
    // no clue why but reading the bmp file like this causes the 2d buffer to be rotated 90 degrees
    // I'm so done trying to fix this mess I'm just gonna rerotate the matrix after reading
    for (int i = 0; i<dib_header.height; i++) {
        file.read(reinterpret_cast<char*>(pixel_data[i]), dib_header.width * sizeof(rgb_sprite));
        file.seekg(row_size - dib_header.width * sizeof(rgb_sprite), std::ios::cur);
    }

    file.close();

    buffer_2D<rgb_sprite> buffer(dib_header.height, dib_header.width);
    for (i32 i = 0; i<dib_header.width; i++) {
        for (i32 j = 0; j<dib_header.height; j++) {
            buffer[j][dib_header.width - 1 - i] = pixel_data[i][j];
        }
    }

    return buffer;
}

#endif
