#ifndef INC_MOB_H
#define INC_MOB_H

#include <stdbool.h>
#include <stdlib.h>

//TODO Only include what this file needs?
#include "../game_cfg/game_cfg.h"
#include "../simulation/simulation.h"
#include "../simulation/vector.h"
#include "../chemistry/chemistry.h"

enum item_type {Weapon, Potion, Creature};

struct InventoryItem;

struct Level;

typedef struct {
    char display;
    char *name;
    constituents *chemistry;
    struct InventoryItem *contents;
    int health;
    enum item_type type;
    struct event_listener listeners[SENSORY_EVENT_COUNT];
} item;

typedef struct InventoryItem {
    item* item;
    struct InventoryItem* next;
} inventory_item;

typedef struct Mobile {
    item base;
    int x;
    int y;
    struct Level *lvl;
    bool active;
    bool stacks;
    char emote;
    void *state;
} mobile;

mobile* make_mob();
void destroy_mob(mobile *mob);
void push_inventory(mobile* mob, item* itm);
item* pop_inventory(mobile* mob);
char* inventory_string(mobile* mob, int len);
void destroy_item(item *itm);

void rotate_inventory(mobile* mob);
bool quaff(mobile* mob);

void item_deal_damage(struct Level *lvl, item* itm, unsigned int amount);

int never_next_firing(void *context, void* mob, struct event_listener *listeners);
void dummy_fire(void *context, void* mob);
int every_turn_firing(void *context, void* mob, struct event_listener *listeners);
void player_move_fire(void *context, void* vmob);
int random_walk_next_firing(void *context, void* mob, struct event_listener *listeners);
void random_walk_fire(void *context, void* mob);
#endif
