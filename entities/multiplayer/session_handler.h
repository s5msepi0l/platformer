#pragma once

#include "../../src/game_engine.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <memory>

#define SERVER_IP "127.0.0.1"
#define PORT 4444

#define BUFFER_SIZE 1024

using namespace game_engine;


enum packet_type : u8 {
    game_state_update = 1
};

/* Removed memory padding to avoid cross platform issues
*/
typedef struct {
    packet_type header;
    u32 len;
    void *data;
}Packet __attribute__((packed));

class session_handler_comp : public component {
public:
    /* Establish connection and create session
    */

   u32 socket_fd;
   struct sockaddr_in server_addr;

    void init() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cout << "unable to init socket_fd\n";
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        inet_aton(SERVER_IP, &server_addr.sin_addr);

        if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cout << "Unable to connect to server\n";
        }
    }
    
    void tick() {}

    void stop() {}

	void collision_enter(entity *other) {};
    void collision_exit(entity *other) {};
};

class session_handler : public entity {
public:
    session_handler() {
        add_component(std::move(std::make_unique<session_handler_comp>()));
    }
};