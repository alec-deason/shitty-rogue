#ifndef COLOR_H
#define COLOR_H

#include <ncurses.h>

#define REQUIRED_COLORS 256
#define REQUIRED_COLOR_PAIRS 256

// Custom colors
//TODO should I not use the COLOR_ prefix to make it clear
//TODO that this is not a built-in color?
#define COLOR_BROWN 17
//TODO this 17 and the one below are different... is this dumb?
#define COLOR_DARK_GRAY 18
#define COLOR_LIGHT_GRAY 19

// Color pair IDs
#define ORIG_COLORS -1
#define PAIR_BLACK 0
#define PAIR_RED 1
#define PAIR_GREEN 2
#define PAIR_YELLOW 3
#define PAIR_BLUE 4
#define PAIR_MAGENTA 5
#define PAIR_CYAN 6
#define PAIR_WHITE 7
#define PAIR_BROWN 17
#define PAIR_DARK_GRAY 18
#define PAIR_LIGHT_GRAY 19

// Color pairs (foreground and background)
#define BLACK COLOR_PAIR(PAIR_BLACK)
#define RED COLOR_PAIR(PAIR_RED)
#define GREEN COLOR_PAIR(PAIR_GREEN)
#define YELLOW COLOR_PAIR(PAIR_YELLOW)
#define BLUE COLOR_PAIR(PAIR_BLUE)
#define MAGENTA COLOR_PAIR(PAIR_MAGENTA)
#define CYAN COLOR_PAIR(PAIR_CYAN)
#define WHITE COLOR_PAIR(PAIR_WHITE)
#define BROWN COLOR_PAIR(PAIR_BROWN)
#define DARK_GRAY COLOR_PAIR(PAIR_DARK_GRAY)
#define LIGHT_GRAY COLOR_PAIR(PAIR_LIGHT_GRAY)

// Functions
bool init_colors();

#endif
