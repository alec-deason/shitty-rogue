#ifndef INPUT_H
#define INPUT_H

#include "level/level.h"

// This reproduces a portion of the curses keymap without needing the
// entire ncurses library.
#define KEY_DOWN    0402
#define KEY_UP      0403
#define KEY_LEFT    0404
#define KEY_RIGHT   0405

int get_keystroke(void);
void get_input(level *lvl);
#endif
