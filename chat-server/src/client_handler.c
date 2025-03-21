#include "../inc/server.h"

void *handle_client(void *arg) {
    int socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    Client *client = malloc(sizeof(Client));
    
    if (!client) {
        perror("Memory allocation failed");
        close(socket);
        return NULL;
    }
    
    client->socket = socket;
    
    // Receive username from client
    int bytes_received = recv(socket, client->username, sizeof(client->username) - 1, 0);
    if (bytes_received <= 0) {
        perror("Username receive failed");
        free(client);
        close(socket);
        return NULL;
    }
    client->username[bytes_received] = '\0';  // Ensure null termination
    
    // Get client IP address
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getpeername(socket, (struct sockaddr *)&addr, &addr_len);
    inet_ntop(AF_INET, &addr.sin_addr, client->ip, sizeof(client->ip));
    
    // Add client to the list
    add_client(client);
    
    printf("New client connected: %s (%s)\n", client->username, client->ip);
    
    while (1) {
        // Receive message from client
        bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break; // Client disconnected
        }
        buffer[bytes_received] = '\0';  // Ensure null termination
        
        // Check for ">>bye<<" message
        if (strcmp(buffer, ">>bye<<") == 0) {
            break; // Graceful shutdown for this client
        }
        
        // Broadcast message to all clients
        broadcast_message(buffer, socket, client->username, client->ip);
    }
    
    // Remove client from the list
    remove_client(socket);
    close(socket);
    printf("Client disconnected: %s (%s)\n", client->username, client->ip);
    
    // Check if this was the last client
    pthread_mutex_lock(&client_list_mutex);
    if (client_list == NULL) {
        printf("No more clients connected. Server can be shut down.\n");
    }
    pthread_mutex_unlock(&client_list_mutex);
    
    return NULL;
}