#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "../game.h"
#include "mob.h"
#include "../level/level.h"

mobile* make_mob(struct Level *lvl) {
    mobile *mob = malloc(sizeof(mobile));
    ((item*)mob)->display = ICON_UNDEFINED;
    ((item*)mob)->chemistry = make_constituents();
    ((item*)mob)->type = Creature;
    mob->state = NULL;
    for (int i = 0; i < SENSORY_EVENT_COUNT; i++) ((item*)mob)->listeners[i].handler = NULL;
    mob->lvl = lvl;
    mob->x = 0;
    mob->y = 0;
    ((item*)mob)->health = 1;
    mob->active = false;
    mob->stacks = false;
    mob->emote = false;
    ((item*)mob)->contents = NULL;
    return mob;
}
void destroy_mob(mobile *mob) {
    destroy_constituents(((item*)mob)->chemistry);
    inventory_item *inv = ((item*)mob)->contents;
    while (inv != NULL) {
        inventory_item *next = inv->next;
        free((void*)inv);
        inv = next;
    }
    free((void*)((item*)mob)->name);
    if (mob->state != NULL) {
        free(mob->state);
    }
    free((void*)mob);
}

void push_inventory(mobile* mob, item* itm) {
    inventory_item *new_entry = malloc(sizeof(inventory_item));
    new_entry->next = NULL;
    new_entry->item = itm;

    if (((item*)mob)->contents == NULL) {
        ((item*)mob)->contents = new_entry;
    } else {
        inventory_item *inv = ((item*)mob)->contents;
        while (inv->next != NULL) inv = inv->next;
        inv->next = new_entry;
    }
}

item* pop_inventory(mobile *mob) {
    if (((item*)mob)->contents == NULL) {
        return NULL;
    } else {
        inventory_item *old = ((item*)mob)->contents;
        ((item*)mob)->contents = old->next;
        item *itm = old->item;
        free(old);
        return itm;
    }
}

char* inventory_string(mobile* mob, int len) {
    char *str = malloc(len*sizeof(char));
    str[0] = '\0';
    int total = 0;
    inventory_item *inv = ((item*)mob)->contents;
    bool first = true;

    while (inv != NULL) {
        inventory_item *next = inv->next;
        total += snprintf(str+total, len-total, first ? "%s" : ", %s", inv->item->name);
        first = false;
        if (total >= len) break;
        inv = next;
    }
    return str;
}

void destroy_item(item *itm) {
    destroy_constituents(itm->chemistry);
    free((void*)itm);
}

int never_next_firing(void *context, void* mob, struct event_listener *listeners) {
    return INT_MAX;
}

void dummy_fire(void *context, void* mob) {
}

int every_turn_firing(void *context, void* mob, struct event_listener *listeners) {
    return TICKS_PER_TURN;
}

void player_move_fire(void *context, void* vmob) {
    mobile *mob = (mobile*)vmob;
    int x = mob->x + mob->lvl->keyboard_x;
    int y = mob->y + mob->lvl->keyboard_y;
    mob->lvl->keyboard_x = 0;
    mob->lvl->keyboard_y = 0;

    if (x != mob->x || y != mob->y) {
        if (!(move_if_valid(mob->lvl, mob, x, y))) {
            mob->emote = EMOTE_OUCH;
        }
    }
}

int random_walk_next_firing(void *context, void* vmob, struct event_listener *listeners) {
    float rate = 0.5;
    float r = ((float)rand()) / RAND_MAX;
    int next_fire = log(1-r)/(-rate) * TICKS_PER_TURN;
    if (next_fire < TICKS_PER_TURN) return TICKS_PER_TURN;
    return next_fire;
}

void random_walk_fire(void *context, void* vmob) {
    mobile *mob = (mobile*)vmob;
    int x = mob->x;
    int y = mob->y;

    if (rand()%2 == 0) {
        x += rand()%3 - 1;
    } else {
        y += rand()%3 - 1;
    }

    if (x != mob->x || y != mob->y) {
        if (!(move_if_valid(mob->lvl, mob, x, y))) {
            mob->emote = EMOTE_OUCH;
        }
    }
}

void item_deal_damage(level* lvl, item* itm, unsigned int amount) {
    itm->health -= amount;
    if (itm->listeners[DAMAGE].handler != NULL) {
        simulation_call_event_handler(lvl->sim  , &itm->listeners[DAMAGE]);
    }
}
