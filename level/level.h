#ifndef INC_LEVEL_H
#define INC_LEVEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "../game.h"
#include "../mob/mob.h"
#include "../chemistry/chemistry.h"
#include "../simulation/simulation.h"

typedef struct Level {
    unsigned char **tiles;
    unsigned char **memory;
    inventory_item ***items;
    constituents ***chemistry;
    chemical_system *chem_sys;
    int keyboard_x, keyboard_y;
    struct simulation *sim;
    int width;
    int height;
    mobile **mobs;
    int mob_count;
    mobile *player;
    bool active;
} level;


static bool approach(level *lvl, mobile *actor, int target_x, int target_y);
level* make_level(void);
void destroy_level(level *lvl);
void level_push_item(level *lvl, item *itm, int x, int y);
item* level_pop_item(level *lvl, int x, int y);


static bool one_step(level *lvl, int *from_x, int *from_y, int to_x, int to_y);
bool is_position_valid(level *lvl, int x, int y);
bool move_if_valid(level *lvl, mobile *mob, int x, int y);

#endif
