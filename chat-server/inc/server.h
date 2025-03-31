/*
 * FILE: server.h
 * PROJECT: Chat Server Application
 * PROGRAMMER: Manreet Thind
 * FIRST VERSION: 31-03-2025
 * DESCRIPTION:
 * This header file contains all necessary definitions, structures, and function prototypes
 * for the chat server implementation. It manages client connections, message broadcasting,
 * and thread synchronization for a multi-client chat server.
 * Resources:
 * https://www.prepbytes.com/blog/arrays/two-dimensional-array-of-characters/  
 */
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

//Global variables
extern Client *client_list; /*Required across all connection handlers for broadcast operations and client management.*/
extern pthread_mutex_t client_list_mutex;/*Must be shared by all threads accessing client_list*/
extern int active_threads;/*System-wide thread count needed for server shutdown coordination*/
extern pthread_mutex_t thread_count_mutex;/*Ensures increments/decrements of active_threads from any thread context*/

// Function prototypes
void *handle_client(void *arg);
void broadcast_message(char *message, int sender_socket, char *sender_username, char *sender_ip);
void add_client(Client *client);
void remove_client(int socket);
void cleanup();
void split_message(char *message, char chunks[][MESSAGE_CHUNK_SIZE + 1], int *num_chunks); // 2-D character array  
void get_timestamp(char *timestamp);

#endif