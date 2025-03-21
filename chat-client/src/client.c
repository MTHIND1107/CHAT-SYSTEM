#include "../inc/client.h"

// Global variables
char my_ip[16];
char my_username[6];

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

        // Check for exit command
        if (strcmp(input_buffer, ">>bye<<") == 0) {
            if (send(socket_fd, input_buffer, strlen(input_buffer), 0) < 0) {
                perror("Failed to send exit message");
            }
            break;
        }

        // Send message to server
        if (send(socket_fd, input_buffer, strlen(input_buffer), 0) < 0) {
            perror("Failed to send message");
            break;
        }

        // Add outgoing message to our own display
        char timestamp[9];
        get_timestamp(timestamp);

        // Calculate safe size for message
        size_t max_input_len = BUFFER_SIZE - snprintf(NULL, 0, "%s_[%s]>>_(%s)", my_ip, my_username, timestamp) - 1;

        if (strlen(input_buffer) > max_input_len) {
            input_buffer[max_input_len] = '\0'; // Truncate safely
        }

        // Safe message formatting
        snprintf(send_buffer, BUFFER_SIZE - 1, "%s_[%s]>>%s_(%s)", my_ip, my_username, input_buffer, timestamp);
        send_buffer[BUFFER_SIZE - 1] = '\0';

        display_message(send_buffer);
    }

    // Cleanup
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    close(socket_fd);
    endwin();
    printf("Client shut down gracefully.\n");
    return 0;
}


// Get Client IP address based on the connected socket
void get_client_ip(int socket) {
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    if (getsockname(socket, (struct sockaddr *)&local_addr, &addr_len) < 0) {
        perror("Failed to get socket name");
        return;
    }

    inet_ntop(AF_INET, &local_addr.sin_addr, my_ip, sizeof(my_ip));

    if (strcmp(my_ip, "127.0.0.1") == 0) {
        struct ifaddrs *ifaddr, *ifa;

        if (getifaddrs(&ifaddr) != -1) {
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
                    char *ip = inet_ntoa(addr->sin_addr);

                    if (strcmp(ip, "127.0.0.1") != 0) {
                        strncpy(my_ip, ip, sizeof(my_ip) - 1);
                        break;
                    }
                }
            }
            freeifaddrs(ifaddr);
        }
    }
}