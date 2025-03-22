#include "../inc/server.h"

// Split a message into 40-character chunks, respecting word boundaries
void split_message(char *message, char chunks[][MESSAGE_CHUNK_SIZE + 1], int *num_chunks) {
    int len = strlen(message);
    int current_chunk = 0;
    int last_space = -1;
    int chunk_start = 0;
    
    *num_chunks = 0;
    
    for (int i = 0; i < len; i++) {
        // Track the last space position
        if (message[i] == ' ') {
            last_space = i;
        }
        
        // When we hit the chunk size or end of string
        if ((i - chunk_start == MESSAGE_CHUNK_SIZE - 1) || (i == len - 1)) {
            int chunk_end;
            
            // If we're at the end of the string, include the last character
            if (i == len - 1) {
                chunk_end = i + 1;
            } 
            // If we found a space in this chunk, break at the space
            else if (last_space >= chunk_start && (i - last_space) < 10) {
                chunk_end = last_space;
                i = last_space;  // Reset position to last space
            } 
            // Otherwise, just break at the chunk size
            else {
                chunk_end = i + 1;
            }
            
            // Copy this chunk
            int chunk_length = chunk_end - chunk_start;
            if (chunk_length > 0) {
                strncpy(chunks[current_chunk], message + chunk_start, chunk_length);
                chunks[current_chunk][chunk_length] = '\0';
                current_chunk++;
                chunk_start = chunk_end;
                if (message[chunk_end] == ' ') {
                    chunk_start++;  // Skip the space at the beginning of next chunk
                }
            }
        }
    }
    
    *num_chunks = current_chunk;
    
    // If we have an empty message, create one empty chunk
    if (*num_chunks == 0) {
        strcpy(chunks[0], "");
        *num_chunks = 1;
    }
}