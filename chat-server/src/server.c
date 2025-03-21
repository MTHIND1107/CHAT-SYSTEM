#include "../inc/server.h"

Client *client_list = NULL;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Create a new thread for the client
        pthread_t thread_id;
        int *socket_ptr = malloc(sizeof(int));
        if (socket_ptr == NULL) {
            perror("Memory allocation failed");
            close(new_socket);
            continue;
        }
        *socket_ptr = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)socket_ptr) < 0) {
            perror("Thread creation failed");
            close(new_socket);
            free(socket_ptr);
        }
    }

    // Cleanup before shutting down
    cleanup();
    close(server_fd);
    printf("Server shut down gracefully.\n");
    return 0;
}