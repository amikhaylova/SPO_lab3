//
// Created by alena on 15.11.2021.
//

#include "gui.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int arrow_coord = 0;
int board_max = 0;
struct coordinates *start_with;
char *board_white_string;
char *message_white_string;
char c_[2];

#define BOARD_HEIGHT 17


void update() {
    printf("\033[H\033[J");
}

void goto_xy(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

char *init_white_string(size_t size) {
    char *white_string = malloc(size);
    for (int i = 0; i < size; i++) white_string[i] = ' ';
    return white_string;
}

void update_arrow(int y_next) {
    goto_xy(77, arrow_coord);
    puts("   ");
    goto_xy(77, y_next);
    puts("<--");
    arrow_coord = y_next;
}

void show_possible_commands() {
    goto_xy(0, BOARD_HEIGHT + 1);
    puts("ESC - change mode (write message / interact with tree), q - exit");
    puts("INTERACT: wasd - navigate, e - expand(collapse) node");
    puts("WRITE: ENTER - send msg, BACKSPACE - delete char");
}

void init_screen() {
    set_non_canonical_terminal_mode();
    update();
    board_white_string = init_white_string(75);
    message_white_string = init_white_string(80);
    start_with = malloc(sizeof(struct coordinates));
    start_with->node_id = 0;
    start_with->column = 0;
    goto_xy(0, 0);

    for (int i = 0; i < 80; i++) {
        goto_xy(i, BOARD_HEIGHT);
        puts("-");
    }
    update_arrow(0);
    show_possible_commands();
}

void
order_draw_node(struct tree *message_tree, struct visibility_status *visible, struct draw_status *draw_order, int level,
                int id) {
    struct tree_node *node = &(message_tree->start)[id];
    draw_order[board_max].id = node->id;
    draw_order[board_max].level = level;
    if (visible[id].new_message) {
        draw_order[board_max].new_message = 1;
        visible[id].new_message = 0;
    }
    board_max++;
    if (visible[id].visible) {
        if (node->child_id > 0) {
            order_draw_node(message_tree, visible, draw_order, level + 1, node->child_id);
        }
    }
    if (node->sibling_id > 0) {
        order_draw_node(message_tree, visible, draw_order, level, node->sibling_id);
    }
}

void order_draw_tree(struct tree *message_tree, struct visibility_status *visible, struct draw_status *draw_order) {
    board_max = 0;
    order_draw_node(message_tree, visible, draw_order, 0, 0);
}

int calc_row_length(struct tree *message_tree, struct draw_status *draw_status_element) {
    struct tree_node *node = &(message_tree->start)[draw_status_element->id];
    int result = draw_status_element->level * 3 + 2 + 17 + 1 + strlen(node->username) + 3 + strlen(node->content);
    return result;
}

char *create_row(struct tree *message_tree, struct draw_status *draw_status_element) {
    struct tree_node *node = &(message_tree->start)[draw_status_element->id];
    int len = calc_row_length(message_tree, draw_status_element);
    char *row = malloc(len + 1);
    int i;
    for (i = 0; i < draw_status_element->level; i++) {
        row[3 * i] = ' ';
        row[3 * i + 1] = ' ';
        row[3 * i + 2] = ' ';
    }
    row[3 * draw_status_element->level] = '\0';
    if (draw_status_element->new_message) {
        strcat(row, "!-");
    } else {
        strcat(row, "+-");
    }

    struct tm *tm_info;
    tm_info = localtime(&node->creation_time);
    i = draw_status_element->level * 3 + 2;
    strftime(&row[i], 18, "%D %T", tm_info);
    strcat(row, " ");
    strcat(row, node->username);
    strcat(row, " - ");
    strcat(row, node->content);
    row[len] = '\0';
    return row;
}

void flush_board() {
    for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
        goto_xy(0, i);
        puts(board_white_string);
    }
}

void draw_tree(struct tree *message_tree, struct draw_status *draw_order) {
    int draw_order_i = 0;
    while (draw_order[draw_order_i].id != start_with->node_id) draw_order_i++;
    int board_i = 0;
    flush_board();
    char *row;
    for (; draw_order_i < board_max && board_i < BOARD_HEIGHT - 1; draw_order_i++) {
        goto_xy(0, board_i);
        row = create_row(message_tree, &draw_order[draw_order_i]);
        if (strlen(row) > start_with->column) {
            if (strlen(&row[start_with->column]) > 75) {
                row[start_with->column + 75] = '\0';
            }
            puts(&row[start_with->column]);
        } else {
            puts(board_white_string);
        }
        board_i++;
    }
}

void redraw_tree(struct tree *client_message_tree, struct visibility_status *visible, struct draw_status *draw_order) {
    order_draw_tree(client_message_tree, visible, draw_order);
    draw_tree(client_message_tree, draw_order);
}

int get_current_node(struct draw_status *draw_order) {
    int draw_order_i = 0;
    while (draw_order[draw_order_i].id != start_with->node_id) draw_order_i++;
    draw_order_i += arrow_coord;
    if (draw_order_i < board_max) {
        return draw_order[draw_order_i].id;
    }
    return -1;
}

void remove_new_message(struct draw_status *draw_order) {
    int draw_order_i = 0;
    while (draw_order[draw_order_i].id != start_with->node_id) draw_order_i++;
    draw_order_i += arrow_coord;
    if (draw_order_i < board_max) {
        if (draw_order[draw_order_i].new_message) {
            draw_order[draw_order_i].new_message = 0;
            goto_xy(draw_order[draw_order_i].level * 3, arrow_coord);
            puts("+");
        }
    }
}

void remove_symbol(int position) {
    int row = position / MESSAGE_ROW_MAX;
    int column = position % MESSAGE_ROW_MAX;
    goto_xy(column, row + BOARD_HEIGHT + 4);
    puts(" ");
};

void add_symbol(int position, char c) {
    int row = position / MESSAGE_ROW_MAX;
    int column = position % MESSAGE_ROW_MAX;
    goto_xy(column, row + BOARD_HEIGHT + 4);
    sprintf(c_, "%c", c);
    puts(c_);
}

void flush_message() {
    for (int i = 0; i < 2; i++) {
        goto_xy(0, i + BOARD_HEIGHT + 4);
        puts(message_white_string);
    }
}

void up(struct tree *message_tree, struct draw_status *draw_order) {

    if (arrow_coord == 0) {
        int draw_order_i = 0;
        while (draw_order[draw_order_i].id != start_with->node_id) draw_order_i++;
        if (draw_order_i == 0) return;
        start_with->node_id = draw_order[draw_order_i - 1].id;
        draw_tree(message_tree, draw_order);
    } else {
        update_arrow(arrow_coord - 1);
        remove_new_message(draw_order);
    }
}

void down(struct tree *message_tree, struct draw_status *draw_order) {
    if (arrow_coord == BOARD_HEIGHT - 2) {
        int draw_order_i = 0;
        while (draw_order[draw_order_i].id != start_with->node_id) draw_order_i++;
        if (board_max - draw_order_i < BOARD_HEIGHT - 1) return;
        remove_new_message(draw_order);
        start_with->node_id = draw_order[draw_order_i + 1].id;
        draw_tree(message_tree, draw_order);
    } else {
        update_arrow(arrow_coord + 1);
        remove_new_message(draw_order);
    }
};

void right(struct tree *message_tree, struct draw_status *draw_order) {
    int max = 0;
    int len = 0;
    for (int draw_order_i = 0; draw_order_i < board_max; draw_order_i++) {
        len = calc_row_length(message_tree, &(draw_order[draw_order_i]));
        max = len > max ? len : max;
    }
    if (max - start_with->column - 1 - 75 > 0) {
        start_with->column++;
        draw_tree(message_tree, draw_order);
    }
};

void left(struct tree *message_tree, struct draw_status *draw_order) {
    if (start_with->column == 0) return;
    start_with->column--;
    draw_tree(message_tree, draw_order);
}

void roll(struct tree *message_tree, struct visibility_status *visible, struct draw_status *draw_order) {
    int current_node = get_current_node(draw_order);
    visible[current_node].visible = !visible[current_node].visible;
    redraw_tree(message_tree, visible, draw_order);
}
