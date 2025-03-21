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