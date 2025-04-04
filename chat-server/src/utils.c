/*
 * FILE: utils.c
 * PROJECT: Chat Server Application
* PROGRAMMER: Manreet Thind
 * FIRST VERSION: 31-03-2025
 * DESCRIPTION:
 * This file contains utility functions for message processing and time handling
 * used by the chat server.
 *  * Resources:
 * https://www.prepbytes.com/blog/arrays/two-dimensional-array-of-characters/  
 */
#include "../inc/server.h"

/*
 * Name    : split_message
 * Purpose : Split long messages into fixed-size chunks
 * Input   : message - char* - Original message to split
 *           chunks - char[][] - Output array for message chunks
 *           num_chunks - int* - Output count of chunks created
 * Outputs : Fills chunks array with message segments
 * Returns : Nothing
 * Notes   : Preserves word boundaries when possible
 */
void split_message(char *message, char chunks[][MESSAGE_CHUNK_SIZE + 1], int *num_chunks) {
    int len = strlen(message);
    *num_chunks = 0;
    
    // If message is empty, return a single empty chunk
    if (len == 0) {
        strcpy(chunks[0], "");
        *num_chunks = 1;
        return;
    }
    
    int start = 0;
    while (start < len && *num_chunks < 10) {
        // Calculate remaining length
        int remaining = len - start;
        
        // If remaining fits in one chunk
        if (remaining <= MESSAGE_CHUNK_SIZE) {
            strcpy(chunks[*num_chunks], message + start);
            (*num_chunks)++;
            break;
        }

        // Find last space before chunk boundary
        int split_pos = start + MESSAGE_CHUNK_SIZE;
        while (split_pos > start && message[split_pos] != ' ') {
            split_pos--;
        }

        // If no space found within reasonable distance
        if (split_pos <= start + MESSAGE_CHUNK_SIZE - 10) {
            split_pos = start + MESSAGE_CHUNK_SIZE;
        }

        // Copy the chunk
        int chunk_len = split_pos - start;
        strncpy(chunks[*num_chunks], message + start, chunk_len);
        chunks[*num_chunks][chunk_len] = '\0';
        
        // Remove trailing space if present
        if (chunks[*num_chunks][chunk_len-1] == ' ') {
            chunks[*num_chunks][chunk_len-1] = '\0';
        }
        
        (*num_chunks)++;
        start = split_pos + (message[split_pos] == ' ' ? 1 : 0);
    }
}


/*
 * Name    : get_timestamp
 * Purpose : Generate current time as formatted string
 * Input   : timestamp - char* - Output buffer (min 9 bytes)
 * Outputs : Fills buffer with HH:MM:SS formatted time
 * Returns : Nothing
 * Notes   : Uses system local time
 */
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    sprintf(timestamp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}