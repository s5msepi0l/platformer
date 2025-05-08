#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image/stb_image.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <map>
#include <list>
#include <unordered_map>
#include <memory>
#include <sstream>

#include <SDL2/SDL.h>

#if defined(__linux__)
    #define sleep(ms) usleep(ms * 1000)
#else // windows or some other distro ig

#endif

namespace std {
    //generally used for holding ascii values or 
    typedef basic_string<unsigned char> ustring;
}

#define LOG(...) log(__VA_ARGS__)

template<typename T>
void log(T arg) {
    std::cout << arg << std::endl;
}

template<typename T, typename... Args>
void log(T first, Args... args) {
    std::cout << first << " ";
    log(args...);
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

//https://tutorialedge.net/gamedev/aabb-collision-detection-tutorial/
#define AABB(p1x, p1y, p1w, p1h, p2x, p2y, p2w, p2h) \
    (p1x < p2x + p2w &&     \
    p1x + p1w > p2x &&      \
    p1y < p2y + p2h &&      \
    p1y + p1h > p2y         \
    )

typedef struct {
    u8 r = 255;
    u8 g = 255;
    u8 b = 255;
    u8 a = 255;
}rgba;

//general coordinate struct
template <typename T>
struct vector2 {
    T x;
    T y;

    vector2(): x(0), y(0) {}
    vector2(T X, T Y): x(X), y(Y) {}


    // √(x_2-x_1)²+(y_2-y_1)²
    inline T distance_to(vector2 other) const {
        return static_cast<T>(sqrt(pow(other.x, 2) + pow(other.y, 2)));
    }

    inline T distance_to(T x_pos, T y_pos) const {
        return static_cast<T>(sqrt(pow(x_pos, 2) + pow(y_pos, 2)));
    }

    void operator+=(vector2 other) {
        x += other.x;
        y += other.y;
    }

    bool operator==(const vector2& other) const {
        return (x == other.x && y == other.y);
    }

    vector2<T> operator*(f32 other) {
        return vector2<T>{other * x, other * y};
    }

    vector2<T> operator*(vector2 other) {
        return vector2<T>{other.x * x, other.y * y};
    }

    vector2<T> operator+(vector2 other) {
        return vector2<T>{other.x + x, other.y + y};
    }
    
    vector2<T> operator-(vector2 other) {
        return vector2<T>{x - other.x, y - other.y};
    }
};

using vec2  = vector2<f32>;

using uvec32 = vector2<u32>;
using svec32 = vector2<i32>;

using uvec16 = vector2<u16>;
using svec16 = vector2<i16>;

// Define a custom hash function for vec2
namespace std {
    template <>
    struct hash<vec2> {
        std::size_t operator()(const vec2& v) const {
            // Use a simple combination of x and y for the hash value
            std::size_t h1 = std::hash<float>{}(v.x);
            std::size_t h2 = std::hash<float>{}(v.y);
            
            // Combine the two hash values to get a single hash
            return h1 ^ (h2 << 1); // XOR with left shift
        }
    };
}



// basic rectangular hitbox
typedef struct {
    vec2 pos;
    vec2 size;

    i32 z_index;
}Transform;

//list of Transform(s)
typedef struct hitbox{
    u32 size;
    Transform *data;

    hitbox():
    size(0), data(nullptr) {}

    hitbox(u32 n) {
        size = n;
        data = new Transform[n];
    }

    ~hitbox() {
        delete data;
    }

}hit_box;

class keyboard_input {
public:
    bool quit = false;

    SDL_Event event;

    const u8 *key_states = nullptr;
    struct {
        u32 buttons;
        i32 x, y;
    }mouse;


    //call once at the beginning of every frame
    inline void poll() {
        // Get the state of all keys
        key_states = SDL_GetKeyboardState(NULL);

        mouse.buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
    }

    vec2 poll_axis() {
        vec2 ret;
        // Check if the keys are pressed using SDL_GetKeyState
        if (key_states[SDL_SCANCODE_UP]) {
            ret.y -= 1.0;
        }
        if (key_states[SDL_SCANCODE_DOWN]) {
            ret.y += 1.0;
        }
        if (key_states[SDL_SCANCODE_LEFT]) {
            ret.x -= 1.0;
        }
        if (key_states[SDL_SCANCODE_RIGHT]) {
            ret.x += 1.0;
        }

        return ret;
    }

    vec2 poll_mousepos() {
        vec2 ret;

        ret.x = static_cast<f32>(mouse.x);
        ret.y = static_cast<f32>(mouse.y);

        return ret;
    }
    
    /*
    * - Button 1:  Left mouse button "SDL_BUTTON_LMASK"
    * - Button 2:  Middle mouse button "SDL_BUTTON_MMASK"
    * - Button 3:  Right mouse button "SDL_BUTTON_RMASK"
    * */
    bool poll_mousebutton(i32 mask) {
        return (mouse.buttons & mask);
    }

    bool poll(SDL_Scancode key) {
        return key_states[key];
    }
};

template <typename T>
class buf2 {
private:
    std::vector<T> data;

public:
    u32 width = 0;
    u32 height = 0;

    buf2() {}

    //preallocate memory to it (x * y * sizeof(rgba))
    buf2(u32 w, u32 h):
    data(w * h),
    width(w),
    height(h)
    {}

    void reserve(u32 w, u32 h) {
        if (width == 0 && height == 0) { // not preallocated yet
            data.reserve(w * h);
            
            width =  w;
            height = h;   
        }
    }

    // get direct data pointer
    std::vector<T> ref() const { return this->data; }
    inline bool is_empty() const { return data.empty(); }

    T *operator[](u32 index) {
        return &this->data[index * width];
    }

    const T *operator[](u32 index) const {
        return &data[index * width];
    }

    // shi i might be retarded
    T *get(u32 x, u32 y){
        if (x >= width || y >= height) {
            throw std::out_of_range("index out of bounds");
        }
        return &data[y * width + x];
    }

    void set(u32 x, u32 y, const T& value) {
        if (x >= width || y >= height) {
            throw std::out_of_range("index out of bounds");
        }

        data[y * width + x] = value;
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