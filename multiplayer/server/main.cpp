#include "server.h"

int main(int argc, const char *argv[]) {
    Server server(2222);
    server.run();

    return 0;
}