#include "../inc/client.h"

// Split a message into 40-character chunks
void split_message(char *message, char chunks[][41], int *num_chunks) {
    int len = strlen(message);
    *num_chunks = 0;
    for (int i = 0; i < len; i += 40) {
        strncpy(chunks[*num_chunks], message + i, 40);
        chunks[*num_chunks][40] = '\0'; // Ensure null termination
        (*num_chunks)++;
    }
}

// Format incoming message for display
void format_message(char *buffer, char *ip, char *username, char *message, char *timestamp) {
    snprintf(buffer, BUFFER_SIZE, "%s_[%s]_>>_%s_(%s)", ip, username, message, timestamp);
}

// Get current timestamp in HH:MM:SS format
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    sprintf(timestamp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}