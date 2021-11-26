//
// Created by alena on 15.11.2021.
//

#ifndef SPO_LAB3_CLIENT_H
#define SPO_LAB3_CLIENT_H

#include "../model/model.h"
#include "../gui/gui.h"

#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <poll.h>
#include <termios.h>
#include <signal.h>


enum {
    INTERACT_VIEW = 0,
    WRITE_VIEW = 1,
};

enum {
    MAX_MESSAGE_SIZE = MESSAGE_ROW_MAX * 2,
};

struct buffer {
    char content[225];
    int used;
};

void client_mode(char *user_name, int port);

#endif //SPO_LAB3_CLIENT_H
