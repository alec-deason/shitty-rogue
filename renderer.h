#ifndef CURSES_RENDERER_H
#define CURSES_RENDERER_H

#include "game.h"
#include "level/level.h"
#include "los/los.h"

void init_rendering_system(void);
void cleanup_rendering_system(void);
void draw_level(level *lvl);
void print_message(char *msg);

#endif
