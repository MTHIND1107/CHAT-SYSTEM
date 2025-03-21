#include "../inc/client.h"

// Global variables for UI
WINDOW *input_win;
WINDOW *output_win;
int output_lines = 0;
int max_output_lines = 10;
char message_history[10][BUFFER_SIZE];

// External variables declared in client.c
extern char my_ip[16];
extern char my_username[6];