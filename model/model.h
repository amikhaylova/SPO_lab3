//
// Created by alena on 15.11.2021.
//

#ifndef SPO_LAB3_MODEL_H
#define SPO_LAB3_MODEL_H

#include <bits/types/time_t.h>
#include <stdlib.h>
#include "../gui/gui.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MESSAGE_ROW_MAX 80

struct message {
    char username[20];
    char content[256];
    int parent_id;
};

struct tree_node {
    int id;
    int sibling_id;
    int child_id;
    int parent_id;
    time_t creation_time;
    char username[20];
    char content[256];
};

struct tree {
    struct tree_node *start;
    size_t used;
    size_t size;
};

void insert_tree_node(struct tree *message_tree, struct draw_status *draw_order, struct visibility_status *visible,
                      struct tree_node *node);

int insert_tree_message(struct tree *message_tree, struct message *received_message);

struct draw_status *init_ordered_draw_array(size_t init_size);

struct visibility_status *init_visibility_array(size_t init_size);

struct tree *init_tree(size_t init_size, int server);


#endif //SPO_LAB3_MODEL_H
