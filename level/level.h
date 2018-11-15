#ifndef INC_LEVEL_H
#define INC_LEVEL_H

#include <stdbool.h>
#include <stdlib.h>

#include "../game.h"
#include "../mob/mob.h"
#include "../chemistry/chemistry.h"
#include "../simulation/simulation.h"

typedef struct Level {
    int **tiles; // int to accomodate ncurses attributes
    int **memory;
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

static void make_map(level *lvl);
static int partition(int **room_map, int x, int y, int w, int h, int rm);

level* make_level(void);
void destroy_level(level *lvl);

void level_push_item(level *lvl, item *itm, int x, int y);
item* level_pop_item(level *lvl, int x, int y);

static bool approach(level *lvl, mobile *actor, int target_x, int target_y);
static bool one_step(level *lvl, int *from_x, int *from_y, int to_x, int to_y);

bool is_position_valid(level *lvl, int x, int y);
bool move_if_valid(level *lvl, mobile *mob, int x, int y);

static void minotaur_fire(void *context, void* vmob);
static void umber_hulk_fire(void *context, void* vmob);
static bool umber_hulk_invalidation(void *vmob);
static int umber_hulk_next_firing(void *context, void* vmob, struct event_listener *listeners);

#endif
