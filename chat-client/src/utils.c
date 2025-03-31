/*
 * FILE: utils.c
 * PROJECT: Chat Client Application
 * PROGRAMMER: [Your Name]
 * FIRST VERSION: [Date]
 * DESCRIPTION:
 * This file contains helper utility functions for the chat client including
 * timestamp generation and other supporting functions.
 */
#include "../inc/client.h"

/*
 * Name    : get_timestamp
 * Purpose : Get current timestamp as formatted string
 * Input   : timestamp - char* - Output buffer (must be at least 9 bytes)
 * Outputs : Fills timestamp buffer
 * Returns : Nothing
 * Notes   : Generates timestamp in HH:MM:SS format using system time
 */
void get_timestamp(char *timestamp) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    sprintf(timestamp, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}