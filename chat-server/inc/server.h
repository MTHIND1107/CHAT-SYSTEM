#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MESSAGE_CHUNK_SIZE 40

typedef struct Client {
    int socket;
    char username[6]; // Username (up to 5 characters + null terminator)
    char ip[16];      // IP address (XXX.XXX.XXX.XXX format)
    struct Client *next;
} Client;

extern Client *client_list;
extern pthread_mutex_t client_list_mutex;
extern int active_threads;
extern pthread_mutex_t thread_count_mutex;

// Function prototypes
void *handle_client(void *arg);
void broadcast_message(char *message, int sender_socket, char *sender_username, char *sender_ip);
void add_client(Client *client);
void remove_client(int socket);
void cleanup();
void split_message(char *message, char chunks[][MESSAGE_CHUNK_SIZE + 1], int *num_chunks);
void get_timestamp(char *timestamp);

#endif