//
// Created by alena on 15.11.2021.
//


#include "model.h"

struct tree *init_tree(size_t init_size, int server) {
    struct tree *message_tree;
    message_tree = calloc(sizeof(struct tree), 1);
    message_tree->start = calloc(init_size, sizeof(struct tree_node));
    message_tree->used = 0;
    message_tree->size = init_size;
    message_tree->start->id = 0;
    message_tree->start->sibling_id = -1;
    message_tree->start->child_id = -1;
    message_tree->start->parent_id = -1;

    if (server) {
        strcpy(message_tree->start->content, "Board started");
        strcpy(message_tree->start->username, "root");
        message_tree->start->creation_time = time(NULL);
        message_tree->used++;
    }
    return message_tree;
}

struct visibility_status *init_visibility_array(size_t init_size) {
    struct visibility_status *visible = calloc(init_size, sizeof(struct visibility_status));
    return visible;
}

struct draw_status *init_ordered_draw_array(size_t init_size) {
    struct draw_status *draw_order = calloc(init_size, sizeof(struct draw_status));
    return draw_order;
}

int insert_tree_message(struct tree *message_tree, struct message *received_message) {

    //update, if needed, array size
    if (message_tree->used == message_tree->size) {
        message_tree->size *= 2;
        message_tree->start = realloc(message_tree->start, message_tree->size * sizeof(struct tree_node));
    }

    //init new node
    struct tree_node *new_message = &(message_tree->start)[message_tree->used];
    strcpy(new_message->username, received_message->username);
    strcpy(new_message->content, received_message->content);
    new_message->parent_id = received_message->parent_id;
    new_message->child_id = -1;
    new_message->sibling_id = -1;
    new_message->id = (int) message_tree->used;
    new_message->creation_time = time(NULL);

    //find node to change
    struct tree_node *temp = &(message_tree->start)[new_message->parent_id];
    int updated;
    if (temp->child_id == -1) {
        temp->child_id = new_message->id;
        updated = temp->id;
    } else {
        temp = &(message_tree->start)[temp->child_id];
        while (temp->sibling_id != -1) {
            temp = &(message_tree->start)[temp->sibling_id];
        }
        temp->sibling_id = new_message->id;
        updated = temp->id;
    }

    //update used nodes
    message_tree->used++;
    return updated;
}

void insert_tree_node(struct tree *message_tree, struct draw_status *draw_order, struct visibility_status *visible,
                      struct tree_node *node) {

    //update, if needed, array size
    if (message_tree->used == message_tree->size) {
        message_tree->size *= 2;
        message_tree->start = realloc(message_tree->start, message_tree->size * sizeof(struct tree_node));
        draw_order = realloc(draw_order, message_tree->size * sizeof(struct draw_status));
        visible = realloc(visible, message_tree->size * sizeof(struct visibility_status));
    }

    //init new node
    struct tree_node *new_message = &(message_tree->start)[node->id];
    memcpy(new_message, node, sizeof(struct tree_node));

    //new node, not updated
    if (node->id >= message_tree->used) {
        visible[node->id].visible = 1;
        visible[node->id].new_message = 1;
        int parent = node->parent_id;
        while (parent != -1) {
            visible[parent].new_message = 1;
            parent = message_tree->start[parent].parent_id;
        }
        visible[0].new_message = 1;
        //update used nodes
        message_tree->used++;
    }
}