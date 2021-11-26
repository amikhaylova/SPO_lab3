//
// Created by alena on 15.11.2021
//

#ifndef SPO_LAB3_GUI_H
#define SPO_LAB3_GUI_H

#include <bits/types/time_t.h>
#include <stdlib.h>
#include "../model/model.h"

struct coordinates {
    int node_id;
    int column;
};

struct draw_status {
    int id;
    int level;
    int new_message;
};

struct visibility_status {
    int visible;
    int new_message;
};

void init_screen();

void set_non_canonical_terminal_mode();

void set_canonical_terminal_mode();

void update();

void order_draw_tree(struct tree *message_tree, struct visibility_status *visible, struct draw_status *draw_order);

void flush_board();

void draw_tree(struct tree *message_tree, struct draw_status *draw_order);

void up(struct tree *message_tree, struct draw_status *draw_order);

void down(struct tree *message_tree, struct draw_status *draw_order);

void right(struct tree *message_tree, struct draw_status *draw_order);

void left(struct tree *message_tree, struct draw_status *draw_order);

int get_current_node(struct draw_status *draw_order);

void remove_symbol(int position);

void add_symbol(int position, char c);

void flush_message();

void redraw_tree(struct tree *client_message_tree, struct visibility_status *visible, struct draw_status *draw_order);

void goto_xy(int x, int y);

void roll(struct tree *message_tree, struct visibility_status *visible, struct draw_status *draw_order);

#endif //SPO_LAB3_GUI_H
