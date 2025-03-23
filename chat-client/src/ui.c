#include "../inc/client.h"

// Global variables for UI
WINDOW *input_win;
WINDOW *output_win;
int output_lines = 0;
int max_output_lines = 10;
char message_history[10][BUFFER_SIZE];

void init_ui() {
    // Initialize ncurses
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);  // Default
    init_pair(2, COLOR_BLACK, COLOR_RED);    // Headers
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Borders

    // Calculate window sizes
    int message_height = LINES - 4;
    int input_height = 3;

    // Create windows
    output_win = newwin(message_height, COLS, 0, 0);
    input_win = newwin(input_height, COLS, message_height + 1, 0);

    // Enable scrolling for output window
    scrollok(output_win, TRUE);

    // Draw borders with red color
    wattron(output_win, COLOR_PAIR(3));
    wborder(output_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(output_win, COLOR_PAIR(3));

    wattron(input_win, COLOR_PAIR(3));
    wborder(input_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(input_win, COLOR_PAIR(3));

    // Add headers with black on red
    wattron(output_win, COLOR_PAIR(2));
    mvwprintw(output_win, 0, COLS / 2 - 4, " Messages ");
    wattroff(output_win, COLOR_PAIR(2));

    wattron(input_win, COLOR_PAIR(2));
    mvwprintw(input_win, 0, COLS / 2 - 8, " Outgoing Message ");
    wattroff(input_win, COLOR_PAIR(2));

    // Add prompt to input window
    wattron(input_win, COLOR_PAIR(1));
    mvwprintw(input_win, 1, 1, "> ");
    wattroff(input_win, COLOR_PAIR(1));

    // Update windows
    wrefresh(output_win);
    wrefresh(input_win);

    // Move cursor to input position
    wmove(input_win, 1, 3);
    wrefresh(input_win);
}

void add_to_history(char *message) {
    // Shift history up if we've reached max lines
    if (output_lines >= max_output_lines) {
        for (int i = 0; i < max_output_lines - 1; i++) {
            strcpy(message_history[i], message_history[i + 1]);
        }
        strcpy(message_history[max_output_lines - 1], message);
    } else {
        strcpy(message_history[output_lines], message);
        output_lines++;
    }

    // Redraw output window content
    werase(output_win);

    // Draw border
    wattron(output_win, COLOR_PAIR(3));
    wborder(output_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(output_win, COLOR_PAIR(3));

    // Add header
    wattron(output_win, COLOR_PAIR(2));
    mvwprintw(output_win, 0, COLS / 2 - 4, " Messages ");
    wattroff(output_win, COLOR_PAIR(2));

    // Print message history
    for (int i = 0; i < output_lines; i++) {
        mvwprintw(output_win, i + 1, 1, "%s", message_history[i]);
    }

    wrefresh(output_win);
}

void display_message(char *buffer) {
    // Parse message format: XXX.XXX.XXX.XXX_[AAAAA]_>>_aaaa..._(HH:MM:SS)
    char ip[16], username[6], timestamp[9];
    char message[BUFFER_SIZE]; // Use larger buffer for message
    char direction[3]; // To store ">>" or "<<"

    // Extract message components
    sscanf(buffer, "%15s [%5[^]]] %2s %[^(] (%8[^)])",
         ip, username, direction, message, timestamp);

    // Determine if this is my message or someone else's
    if (strcmp(username, my_username) == 0) {
        strcpy(direction, ">>"); // Outgoing message
    } else {
        strcpy(direction, "<<"); // Incoming message
    }

    // Format for display
    char formatted[BUFFER_SIZE];
    snprintf(formatted, BUFFER_SIZE, "%-15s [%-5s] %s %s (%s)", ip, username, direction, message, timestamp);

    add_to_history(formatted);
}