/*
 * FILE: client.c
 * PROJECT: Chat Client Application
 * PROGRAMMER: Manreet Thind
 * FIRST VERSION: 31-03-2025
 * DESCRIPTION:
 * This file contains the main client logic including connection handling, message processing,
 * and user interface management. It implements the core functionality for connecting to the chat server,
 * sending/receiving messages, and managing the client lifecycle.
 */
#include "../inc/client.h"

// Global variables
char my_ip[16];
char my_username[6];

/*
 * Name    : graceful_exit
 * Purpose : Cleanly shutdown the client application
 * Input   : sockfd - int - The socket file descriptor to close
 * Outputs : NONE
 * Returns : Nothing
 * Notes   : Sends shutdown command to server, cleans up ncurses, and exits program
 */
void graceful_exit(int sockfd) {
    // Send the shutdown command
    send(sockfd, ">>bye<<", 7, 0);
    
    // Cleanup
    endwin();  // Restore terminal
    close(sockfd);
    exit(EXIT_SUCCESS);
}

/*
 * Name    : handle_incoming_messages
 * Purpose : Thread function to handle incoming messages from server
 * Input   : arg - void* - Pointer to socket file descriptor
 * Outputs : Displays messages in UI
 * Returns : NULL on thread exit
 * Notes   : Continuously listens for messages from server, processes multi-line messages,
 *           and displays them in the UI. Handles server disconnection.
 */
void *handle_incoming_messages(void *arg) {
    int socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break; // Connection closed
        }
        buffer[bytes_received] = '\0';

       // Handle multi-chunk messages
       char *msg = buffer;
       while (*msg) {
           char *end = strchr(msg, '\n');
           if (end) *end = '\0';
           
           if (strlen(msg) > 0) {
               display_message(msg);
           }
           if (!end) break;
           msg = end + 1;
       }
   }
    // Server disconnected - silent exit
    endwin();
    close(socket);
    pthread_exit(NULL);
}

/*
 * Name    : get_client_ip
 * Purpose : Determine client's IP address
 * Input   : socket - int - Connected socket file descriptor
 * Outputs : Modifies global my_ip variable
 * Returns : Nothing
 * Notes   : Gets the local address from socket connection. If localhost (127.0.0.1),
 *           scans network interfaces to find real IP address.
 */
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

/*
 * Name    : format_outgoing_message
 * Purpose : Format outgoing message with metadata
 * Input   : buffer - char* - Output buffer for formatted message
 *           ip - const char* - Sender's IP address
 *           username - const char* - Sender's username
 *           message - const char* - The message content
 *           timestamp - const char* - Formatted timestamp string
 * Outputs : Fills buffer with formatted message
 * Returns : Nothing
 * Notes   : Creates a standardized message format with all metadata included
 */
void format_outgoing_message(char *buffer, const char *ip, const char *username, const char *message, const char *timestamp) {
    snprintf(buffer, BUFFER_SIZE, "%s [%s] >> %s (%s)", ip, username, message, timestamp);
}