#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>

#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOG(str) std::cout << str << std::endl;

#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8  uint8_t

#define i64 int64_t
#define i32 int32_t
#define i16 int16_t
#define i8  int8_t

#define N_BUFFER_SIZE 1024

// holy size_t of antioch !
constexpr static const size_t max_clients = 32;

/* Removed memory padding to so i can send and receive to and from the client without any
   issues */
enum packet_type : u8 {
    game_state_update = 1
};

typedef struct {
    packet_type header;
    u32 len;
    void *data;
} Packet __attribute__((packed));

typedef struct {

}Session;

class Server {
private:
    i32 opt = 1;
    
    i32 port;

    i32 socket_fd;
    struct sockaddr_in server_addr;

    i32 max_fd;

    fd_set read_fds;
    std::vector<i32> client_sockets;

    bool running = true;

public:
    Server(u32 port, u32 max_clientel = 32):
    client_sockets(max_clients){
        // init socket
        if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("Bind failed");
            close(socket_fd);
            exit(EXIT_FAILURE);  
        }

        if (listen(socket_fd, max_clients) == -1) {
            perror("Listen failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        LOG("[*] Listening on 0.0.0.0:" + std::to_string(port));
    }

    void run() {
        max_fd = socket_fd;
        while (running) {
            FD_ZERO(&read_fds);

            FD_SET(socket_fd, &read_fds);
        
            for (i32 i = 0; i < max_clients; i++ ){
                if (client_sockets[i] > 0) {
                    FD_SET(client_sockets[i], &read_fds);
                    if (client_sockets[i] > max_fd) {
                        max_fd = client_sockets[i];
                    }
                }
            }
        

            i32 activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
            if (activity < 0) {
                perror("select error");
                exit(EXIT_FAILURE);
            }

            if (FD_ISSET(socket_fd, &read_fds)) {
                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                i32 new_socket = accept(socket_fd, (struct sockaddr*)&client_addr, &addrlen);

                if (new_socket >= 0) {
                    LOG("New connection, fd: " + std::to_string(new_socket));

                    for (i32 i = 0; i < max_clients; i++) {
                        if (client_sockets[i] == 0) {
                            client_sockets[i] = new_socket;
                            break;
                        }
                    }
                }
            }
            for (i32 i = 0; i < max_clients; i++) {
                i32 sock = client_sockets[i];
                if (sock > 0 && FD_ISSET(sock, &read_fds)) {
                    char buffer[N_BUFFER_SIZE]{0};
                    size_t bytes_read = recv(sock, buffer, N_BUFFER_SIZE - 1, 0);

                    if (bytes_read <= 0) {
                        LOG("Client (" + std::to_string(sock) + ") has disconnected :(");
                        close(sock);
                        client_sockets[i] = 0;
                    } else {
                        buffer[bytes_read] = '\0';
                        LOG("Received from fd (" + std::to_string(sock) + ") buffer: " + buffer + "\nbuffer length: " + std::to_string(bytes_read));

                        //send 
                        send(sock, buffer, bytes_read, 0);
                    }

                }
            }
        }
    }

private:

};