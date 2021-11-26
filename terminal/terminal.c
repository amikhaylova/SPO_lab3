#include <stdio.h>
#include "terminal.h"

static struct termios stored_settings;

void set_non_canonical_terminal_mode() {
    struct termios new_settings;

    tcgetattr(0, &stored_settings);

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON & ~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0, TCSANOW, &new_settings);

    hide_cursor();
}

void set_canonical_terminal_mode() {
    tcsetattr(0, TCSANOW, &stored_settings);
    show_cursor();
}

void hide_cursor() {
    printf("\e[?25l");
}

void show_cursor() {
    printf("\e[?25h");
}
