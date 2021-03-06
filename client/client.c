//
// Created by alena on 15.11.2021.
//

#include "client.h"
#include "../util/util.h"

struct tree *client_message_tree;
struct visibility_status *visible;
struct draw_status *draw_order;
int server_socket;
int console_status = INTERACT_VIEW;
int client_end = 0;
struct buffer *message_buffer;
struct message *client_message;
struct tree_node *accepted_node;

#define ESC 27
#define BACKSPACE 127


int create_socket(int port) {

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Cannot create socket\n");
        return -1;
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        return -1;
    }

    if (connect(server_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    return server_socket;
}

void end_handler(int dummy) {
    flush_board();
    update();
    goto_xy(0, 0);
    puts("Hope not to see you again :)");
    sleep(1);
    client_message->parent_id = -2;
    send(server_socket, client_message, sizeof(struct message), 0);
    client_end = 1;
    update();
}

void init_receive() {
    client_message_tree = init_tree(256, 0);
    visible = init_visibility_array(256);
    draw_order = init_ordered_draw_array(256);
    message_buffer = malloc(sizeof(struct buffer));
    message_buffer->used = 0;
    struct tree_node *accepted_node = calloc(1, sizeof(struct tree_node));
    struct message *hello_message = calloc(1, sizeof(struct tree_node));
    hello_message->parent_id = -1;
    size_t count;
    if (send(server_socket, hello_message, sizeof(struct message), 0) < 0) {
        perror_die("[ERROR]: Can't send Hello Message");
    }
    if (recv(server_socket, &count, sizeof(size_t), 0) > 0) {
        for (int i = 0; i < count; i++) {
            if (recv(server_socket, accepted_node, sizeof(struct tree_node), 0) > 0) {
                insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
            } else {
                perror_die("[ERROR]: Can't accept tree");
            }
        }
    } else {
        perror_die("[ERROR]: Can't accept tree");
    }
}

void server_closed() {
    flush_board();
    goto_xy(0, 0);
    puts("[INFO]: Server closed");
    sleep(1);
    end_handler(0);
}

void receive() {

    if (recv(server_socket, accepted_node, sizeof(struct tree_node), 0) > 0) {
        insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
    } else {
        server_closed();
        return;
    }
    if (recv(server_socket, accepted_node, sizeof(struct tree_node), 0) > 0) {
        insert_tree_node(client_message_tree, draw_order, visible, accepted_node);
    } else {
        server_closed();
        return;
    }
    redraw_tree(client_message_tree, visible, draw_order);
}

void send_message() {
    if (message_buffer->used > 0) {
        int parent_id = get_current_node(draw_order);
        if (parent_id < 0) return;
        client_message->parent_id = parent_id;
        strcpy(client_message->content, message_buffer->content);
        if (send(server_socket, client_message, sizeof(struct message), 0) < 0) {
            perror_die("[ERROR]: Can't send client message");
        }
        message_buffer->content[0] = '\0';
        message_buffer->used = 0;
        flush_message();
    }
}

void update_message(char c) {
    if (message_buffer->used < MAX_MESSAGE_SIZE) {
        message_buffer->content[message_buffer->used] = c;
        add_symbol(message_buffer->used, c);
        message_buffer->used++;
        message_buffer->content[message_buffer->used] = '\0';
    }
}

void backspace_message() {
    if (message_buffer->used > 0) {
        message_buffer->used--;
        message_buffer->content[message_buffer->used] = '\0';
        remove_symbol(message_buffer->used);
    }
};

void set_write_mode() {
    console_status = WRITE_VIEW;
}

void set_print_mode() {
    console_status = INTERACT_VIEW;
}

void user_handler() {
    char c;
    read(STDIN_FILENO, &c, sizeof c);
    if (console_status == INTERACT_VIEW) {
        switch (c) {
            case 'w':
                up(client_message_tree, draw_order);
                break;
            case 's':
                down(client_message_tree, draw_order);
                break;
            case 'a':
                left(client_message_tree, draw_order);
                break;
            case 'd':
                right(client_message_tree, draw_order);
                break;
            case ESC:
                set_write_mode();
                break;
            case 'e':
                roll(client_message_tree, visible, draw_order);
                break;
            case 'q':
                end_handler(66);
                break;
            default:
                break;
        }
    } else {
        switch (c) {
            case '\n':
                send_message();
                break;
            case BACKSPACE:
                backspace_message();
                break;
            case ESC:
                set_print_mode();
                break;
            case 'q':
                end_handler(66);
                break;
            default:
                update_message(c);
                break;
        }
    }
}

void client_mode(char *user_name, int port) {
    create_socket(port);
    signal(SIGINT, end_handler);
    init_receive();
    init_screen();
    redraw_tree(client_message_tree, visible, draw_order);
    set_print_mode();

    client_message = calloc(1, sizeof(struct tree_node));
    strcpy(client_message->username, user_name);
    accepted_node = calloc(1, sizeof(struct tree_node));

    int ret_poll;
    int ndfs = 2;
    struct pollfd fds[2] = {{fd: 0, events: POLLIN},
                            {fd: server_socket, events: POLLIN}};

    while (!client_end) {
        ret_poll = poll(fds, ndfs, 10);
        if (fds[0].revents != 0) {
            user_handler();
        }
        if (fds[1].revents != 0) {
            receive();
        }
    }
    set_canonical_terminal_mode();
}
