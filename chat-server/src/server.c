#include "../inc/server.h"
#include <fcntl.h>      // For fcntl(), F_GETFL, F_SETFL
#include <unistd.h>     // For usleep()

Client *client_list = NULL;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

//To track active client threads
int active_threads = 0;
pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_running = 1;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int had_connections = 0;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            close(server_fd);
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

    // Set server socket to non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    
    while (server_running) {
        // Accept new connection (non-blocking)
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket > 0) {
            // Check if maximum clients reached
            pthread_mutex_lock(&client_list_mutex);
            int client_count = 0;
            for (Client *client = client_list; client != NULL; client = client->next) {
                client_count++;
            }

            if (client_count >= MAX_CLIENTS) {
                close(new_socket);
                pthread_mutex_unlock(&client_list_mutex);
                continue;
            }
            pthread_mutex_unlock(&client_list_mutex);


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
        } else {
            pthread_detach(thread_id); // Detach so we don't need to join later
            pthread_mutex_lock(&thread_count_mutex);
            active_threads++;
            had_connections = 1;
            pthread_mutex_unlock(&thread_count_mutex);
        }
    
    }

    // Check if all clients have disconnected and we've had connections before
    pthread_mutex_lock(&thread_count_mutex);
    if (active_threads == 0 && had_connections) {
        server_running = 0;
    }
    pthread_mutex_unlock(&thread_count_mutex);
    
    // Small sleep to prevent high CPU usage
    usleep(100000); // 100ms
}
    // Cleanup before shutting down
    cleanup();
    close(server_fd);
    //printf("Server shut down gracefully.\n");
    return 0;
}