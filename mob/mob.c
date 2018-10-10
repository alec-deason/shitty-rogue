#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "../game.h"
#include "mob.h"
#include "../level/level.h"

mobile* make_mob(struct Level *lvl) {
    mobile *mob = malloc(sizeof(mobile));
    ((item*)mob)->display = ' ';
    ((item*)mob)->chemistry = make_constituents();
    ((item*)mob)->type = Creature;
    mob->state = NULL;
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

void rotate_inventory(mobile* mob) {
    if (((item*)mob)->contents != NULL && ((item*)mob)->contents->next != NULL) {
        inventory_item *itm = ((item*)mob)->contents;
        while (itm->next->next != NULL) itm = itm->next;
        inventory_item *last = itm->next;
        itm->next = NULL;
        last->next = ((item*)mob)->contents;
        ((item*)mob)->contents = last;
    }
}

void destroy_item(item *itm) {
    destroy_constituents(itm->chemistry);
    free((void*)itm);
}

bool quaff(mobile* mob) {
    inventory_item* inv = ((item*)mob)->contents;
    if (inv != NULL && inv->item->type == Potion) {
        add_constituents(((item*)mob)->chemistry, inv->item->chemistry);
        destroy_item(inv->item);
        ((item*)mob)->contents = inv->next;
        ((item*)mob)->chemistry->stable = false;
        free((void*)inv);
        return true;
    }
    return false;
}

int never_next_firing(void* mob) {
    return INT_MAX;
}

void dummy_fire(void* mob) {
}

int every_turn_firing(void* mob) {
    return TICKS_PER_TURN;
}

void player_move_fire(void* vmob) {
    mobile *mob = (mobile*)vmob;
    int x = mob->x + mob->lvl->keyboard_x;
    int y = mob->y + mob->lvl->keyboard_y;
    mob->lvl->keyboard_x = 0;
    mob->lvl->keyboard_y = 0;

    if (x != mob->x || y != mob->y) {
        if (!(move_if_valid(mob->lvl, mob, x, y))) {
            mob->emote = OUCH;
        }
    }
}

int random_walk_next_firing(void* vmob) {
    float rate = 0.5;
    float r = ((float)rand()) / RAND_MAX;
    int next_fire = log(1-r)/(-rate) * TICKS_PER_TURN;
    if (next_fire < TICKS_PER_TURN) return TICKS_PER_TURN;
    return next_fire;
}

void random_walk_fire(void* vmob) {
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
            mob->emote = OUCH;
        }
    }
}
