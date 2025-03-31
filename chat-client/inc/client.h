/*
 * FILE: client.h
 * PROJECT: Chat Client Application
 * PROGRAMMER: [Your Name]
 * FIRST VERSION: [Date]
 * DESCRIPTION:
 * This header file contains all necessary includes, defines, function prototypes and global variables
 * for the chat client implementation. It provides the interface for UI handling, network communication,
 * and message processing between the client and server.
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <ncurses.h>
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MESSAGE_CHUNK_SIZE 40

// Function prototypes
void init_ui(void);
void add_to_history(char *message);
void display_message(char *buffer);
void *handle_incoming_messages(void *arg);
void get_timestamp(char *timestamp);
void get_client_ip(int socket);

// Global variables
extern WINDOW *input_win;
extern WINDOW *output_win;
extern char my_ip[16];
extern char my_username[6];

#endif