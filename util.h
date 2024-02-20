#ifndef PLATFORMER_UTIL
#define PLATFORMER_UTIL

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <map>

namespace std {
    //generally used for holding ascii values or 
    typedef basic_string<unsigned char> ustring;
}

#define f32 float
#define f64 double 

#define i8 int8_t
#define u8 uint8_t
#define i16 int16_t  
#define u16 uint16_t  
#define u32 uint32_t 
#define i32 int32_t
#define u64 uint64_t
#define i64 int64_t

//general coordinate struct
typedef struct f_vec_2D {
    f32 x;
    f32 y;

    f_vec_2D(): x(0), y(0) {}
    f_vec_2D(f32 X, f32 Y): x(X), y(Y) {}


    // √(x_2-x_1)²+(y_2-y_1)²
    inline f32 distance_to(f_vec_2D other) const {
        return static_cast<f32>(sqrt(pow(other.x, 2) + pow(other.y, 2)));
    }

    inline f32 distance_to(f32 x_pos, f32 y_pos) const {
        return static_cast<f32>(sqrt(pow(x_pos, 2) + pow(y_pos, 2)));
    }
}f_vec_2D;

//general ascii file reader 
std::string f_read(std::string path) {
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
		return nullptr;

	std::string buffer;
	u8 byte;

	while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
		buffer.push_back(byte);
	}

	file.close();

	return buffer;
}


// yeah it's slow but i doubt it's gonna be doing any reading during runtime
std::vector<std::string> l_read(std::string path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<std::string> buffer;
    std::string line;

    //unable to open file probably due to incorrect path
    if (!file.is_open()) {
        return buffer;
    }

    while (std::getline(file, line)) {
        buffer.push_back(line);
    }

    file.close();
    return buffer;
}


#endif