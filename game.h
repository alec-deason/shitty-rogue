#ifndef GAME_H
#define GAME_H

#include "game_cfg/game_cfg.h"

extern int keyboard_x, keyboard_y;
extern char message_banner[MESSAGE_LENGTH];

typedef enum direction {
    up,
    right,
    down,
    left,
    no_direction
} direction;

#endif
