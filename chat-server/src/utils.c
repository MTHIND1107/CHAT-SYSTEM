#include "../inc/server.h"

void split_message(char *message, char chunks[][MESSAGE_CHUNK_SIZE + 1], int *num_chunks) {
    int len = strlen(message);
    *num_chunks = 0;
    
    // If message is empty, return a single empty chunk
    if (len == 0) {
        strcpy(chunks[0], "");
        *num_chunks = 1;
        return;
    }
    
    // Split into chunks of maximum MESSAGE_CHUNK_SIZE characters
    int chunk_idx = 0;
    int remaining = len;
    int pos = 0;
    
    while (remaining > 0) {
        int chunk_size = (remaining > MESSAGE_CHUNK_SIZE) ? MESSAGE_CHUNK_SIZE : remaining;
        
        // Copy this chunk
        strncpy(chunks[chunk_idx], message + pos, chunk_size);
        chunks[chunk_idx][chunk_size] = '\0';
        
        // Update counters
        pos += chunk_size;
        remaining -= chunk_size;
        chunk_idx++;
        
        // Prevent buffer overflow
        if (chunk_idx >= 10) break;
    }
    
    *num_chunks = chunk_idx;
}

// Get current timestamp in HH:MM:SS format
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    sprintf(timestamp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}