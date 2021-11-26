//
// Created by alena on 15.11.2021.
//

#include "util.h"

void perror_die(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}