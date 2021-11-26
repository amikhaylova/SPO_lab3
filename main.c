#include <string.h>
#include <stdio.h>
#include "client/client.h"
#include "server/server.h"

int validate_port(char *port) {
    unsigned long len = strlen(port);
    if (len > 6) return 0;
    int port_n = 0;
    for (int i = (int) len - 1; i > -1; i--) {
        if (port[i] < '0' || port[i] > '9') return -1;
        port_n = port_n * 10 + port[i] - 48;
    }
    if (port_n > 65535) return -1;
    return port_n;
}

int validate_username(char *name) {
    return strlen(name) < 20;
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "server") == 0) {
        server_mode();
        return 0;
    }

    if (argc >= 4 && strcmp(argv[1], "client") == 0) {
        int port = validate_port(argv[2]);
        int is_name = validate_username(argv[3]);
        if (port < 0) printf("[ERROR]: port must contain only number and less than 65535");
        if (!is_name) printf("[ERROR]: username must be no more than 20 letters or numbers");
        if (port < 0 || !is_name) {
            return 0;
        }
        client_mode(argv[3], port);
        return 0;
    }

    printf("Input args:\n"
           "1. \"server\" - launch server mode\n"
           "2. \"client\" - launch client mode, you should specify args \"connection port\", \"username\"\n"
           "\t full args line: \"client port username\"\n");

}