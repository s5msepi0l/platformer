#ifndef PLATFORMER_UTIL
#define PLATFORMER_UTIL

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <map>
#include <list>
#include <unordered_map>
#include <SDL2/SDL.h>

namespace std {
    //generally used for holding ascii values or 
    typedef basic_string<unsigned char> ustring;
}

//compiler doesn't really like when i try to use a u8 as a boolean so im just gonna use a regular bool 
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

    void operator+=(f_vec_2D other) {
        x += other.x;
        y += other.y;
    }

    inline f_vec_2D operator*(f32 op) {
        return f_vec_2D(this->x * op, this->y * op);
    }

}f_vec_2D;

class keyboard_input {
public:
    inline f_vec_2D poll() {
        // Get the state of all keys
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        f_vec_2D ret;

        // Check if the keys are pressed using SDL_GetKeyState
        if (currentKeyStates[SDL_SCANCODE_UP]) {
            ret.y -= 1.0;
        }
        if (currentKeyStates[SDL_SCANCODE_DOWN]) {
            ret.y += 1.0;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT]) {
            ret.x -= 1.0;
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
            ret.x += 1.0;
        }

        return ret;
    }
};

template <typename T>
class buffer_2D {
private:
    std::vector<T> data;

public:
    int width = 0;
    int height = 0;

    buffer_2D() {}

    //preallocate memory to it (x * y * sizeof(rgb))
    buffer_2D(int w, int h):
    data(w * h),
    width(w),
    height(h)
    {    
    }

    // get direct data pointer
    std::vector<T> ref() const { return this->data; }
    inline bool is_empty() const { return data.empty(); }

    T *operator[](int index) {
        return &this->data[index * width];
    }

    const T *operator[](int index) const {
        return &data[index * width];
    }

};

template <typename key_type, typename value_type>
class lru_cache {
private:
    int capacity;
    std::list<std::pair<key_type, value_type>> cache_list; // Represents the cache as a doubly linked list of key-value pairs
    std::unordered_map<key_type, typename std::list<std::pair<key_type, value_type>>::iterator> cache_map; // Maps keys to iterators in the list

public:
    lru_cache(int capacity) : capacity(capacity) {}

    value_type get(const key_type& key) {
        auto it = cache_map.find(key);

        if (it == cache_map.end()) {
            return value_type(); 
        }

        cache_list.splice(cache_list.begin(), cache_list, it->second);

        return it->second->second;
    }

    void put(const key_type& key, const value_type& value) {
        auto it = cache_map.find(key);
        
        if (it != cache_map.end()) {
            it->second->second = value;
            cache_list.splice(cache_list.begin(), cache_list, it->second);
        } else {
            if (cache_list.size() == capacity) {
                key_type old_key = cache_list.back().first;
                cache_map.erase(old_key);
                cache_list.pop_back();
            }

            cache_list.emplace_front(key, value);
            cache_map[key] = cache_list.begin();
        }
    }
};

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
