/*
 * FILE: main.c
 * PROJECT: Chat Client Application
 * PROGRAMMER: [Your Name] (STUDENT ID: [Your ID])
 * FIRST VERSION: [Date]
 * DESCRIPTION: 
 * This file contains the main entry point for the chat client program. It handles command-line arguments,
 * establishes the network connection to the server, initializes the user interface, and manages the
 * primary message processing loop. The program coordinates between network communication (client.c),
 * user interface (ui.c), and utility functions (utils.c) to provide a complete chat client solution.
 */
#include "../inc/client.h"

/*
 * Name    : main
 * Purpose : Main client entry point
 * Input   : argc - int - Argument count
 *           argv - char** - Argument vector
 * Outputs : NONE
 * Returns : Exit status
 * Notes   : Handles command line arguments, establishes server connection,
 *           initializes UI, and manages message sending thread.
 */
int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "-user") != 0 || strcmp(argv[3], "-server") != 0) {
        fprintf(stderr, "Usage: %s -user <username> -server <server>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get and validate username
    strncpy(my_username, argv[2], sizeof(my_username) - 1);
    my_username[sizeof(my_username) - 1] = '\0';

    if (strlen(my_username) > 5) {
        fprintf(stderr, "Username must be 5 characters or less\n");
        exit(EXIT_FAILURE);
    }

    char *server = argv[4];
    int socket_fd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, server, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Get client IP address
    get_client_ip(socket_fd);

    // Send username to server
    if (send(socket_fd, my_username, strlen(my_username), 0) < 0) {
        perror("Failed to send username");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Initialize UI
    init_ui();

    // Add welcome message
    char welcome[BUFFER_SIZE];
    snprintf(welcome, BUFFER_SIZE, "*** Connected to chat server as %s ***", my_username);
    add_to_history(welcome);

    // Create thread to handle incoming messages
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, handle_incoming_messages, (void *)&socket_fd) != 0) {
        perror("Failed to create thread");
        close(socket_fd);
        endwin();
        exit(EXIT_FAILURE);
    }

    // Main loop to send messages
    char input_buffer[81]; // 80 chars max + null terminator
    char send_buffer[BUFFER_SIZE];

while (1) {
    // Clear input area and redraw prompt
    wmove(input_win, 1, 1);
    wclrtoeol(input_win);
    mvwprintw(input_win, 1, 1, "> ");
    wrefresh(input_win);

    // Get input (up to 80 chars)
    echo();
    wgetnstr(input_win, input_buffer, 80);
    noecho();
    input_buffer[80] = '\0'; // Ensure null termination

    // Check if input is empty
    if (strlen(input_buffer) == 0) {
        continue; // Don't send empty messages
    }

    // Check for shutdown command
    if (strcmp(input_buffer, ">>bye<<") == 0) {
        graceful_exit(socket_fd);
    }

    // Send message to server
    if (send(socket_fd, input_buffer, strlen(input_buffer), 0) < 0) {
        perror("Failed to send message");
        break;
    }

    // Add outgoing message to our own display
    char timestamp[9];
    get_timestamp(timestamp);

    // Format outgoing message
    format_outgoing_message(send_buffer, my_ip, my_username, input_buffer, timestamp);
    display_message(send_buffer);
}

    // Cleanup
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    close(socket_fd);
    endwin();
    //printf("Client shut down gracefully.\n");
    return 0;
}