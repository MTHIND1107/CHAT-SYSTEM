#include "../inc/server.h"

// External declaration for active_threads and mutex
extern int active_threads;
extern pthread_mutex_t thread_count_mutex;

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
    free(arg);
    //printf("Client disconnected: %s (%s)\n", client->username, client->ip);
    
    // Decrement active thread count
    pthread_mutex_lock(&thread_count_mutex);
    active_threads--;
    pthread_mutex_unlock(&thread_count_mutex);
    
    return NULL;
}

void broadcast_message(char *message, int sender_socket, char *sender_username, char *sender_ip) {
    char chunks[10][MESSAGE_CHUNK_SIZE + 1];
    int num_chunks;

    // Split message into 40-character chunks
    split_message(message, chunks, &num_chunks);

    char timestamp[9];
    get_timestamp(timestamp);

    pthread_mutex_lock(&client_list_mutex);
    for (Client *client = client_list; client != NULL; client = client->next) {
        if (client->socket != sender_socket) { // Don't send back to the sender
            for (int i = 0; i < num_chunks; i++) {
                char formatted[BUFFER_SIZE];
                snprintf(formatted, BUFFER_SIZE, "%s [%s] << %s (%s)\n", 
                         sender_ip, sender_username, chunks[i], timestamp);

                // Make sure we're sending the full formatted message
                if (send(client->socket, formatted, strlen(formatted), 0) < 0) {
                    perror("Failed to send message to client");
                }
                usleep(5000); // Small delay between chunks
            }
        }
    }
    pthread_mutex_unlock(&client_list_mutex);
}

void add_client(Client *client) {
    pthread_mutex_lock(&client_list_mutex);
    client->next = client_list;
    client_list = client;
    pthread_mutex_unlock(&client_list_mutex);
}

void remove_client(int socket) {
    pthread_mutex_lock(&client_list_mutex);
    Client **pp = &client_list;
    Client *temp;
    
    while (*pp) {
        if ((*pp)->socket == socket) {
            temp = *pp;
            *pp = (*pp)->next;
            free(temp);
            break;
        }
        pp = &(*pp)->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
}

void cleanup() {
    pthread_mutex_lock(&client_list_mutex);
    while (client_list) {
        Client *temp = client_list;
        client_list = client_list->next;
        close(temp->socket);
        free(temp);
    }
    pthread_mutex_unlock(&client_list_mutex);
}