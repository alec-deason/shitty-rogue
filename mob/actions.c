#include <stdio.h>

#include "actions.h"
#include "../renderer.h"

void mob_toggle_door(level *lvl, mobile *mob, direction dir) {
    int x = mob->x;
    int y = mob->y;
    switch (dir) {
        case up:
            y -= 1;
            break;
        case down:
            y += 1;
            break;
        case right:
            x += 1;
            break;
        case left:
            x -= 1;
            break;
        default:
            return;
    }
    if (lvl->tiles[x][y] == DOOR_OPEN) {
        lvl->tiles[x][y] = DOOR_CLOSED;
    } else if (lvl->tiles[x][y] == DOOR_CLOSED) {
        lvl->tiles[x][y] = DOOR_OPEN;
    }
}

void mob_drop_item(level *lvl, mobile *mob) {
    item *item = pop_inventory(mob);
    if (item != NULL) {
        level_push_item(lvl, item, mob->x, mob->y);
        if (mob == lvl->player) {
            char msg[MESSAGE_LENGTH];
            snprintf(msg, MESSAGE_LENGTH, "You drop the %s", item->name);
            print_message(msg);
        }
    }
}

void mob_pickup_item(level *lvl, mobile *mob) {
    item *item = level_pop_item(lvl, mob->x, mob->y);
    if (item != NULL) {
        push_inventory(mob, item);
        if (mob == lvl->player) {
            char msg[MESSAGE_LENGTH];
            snprintf(msg, MESSAGE_LENGTH, "You pick up the %s", item->name);
            print_message(msg);
        }
    }
}

void mob_smash_potion(level *lvl, mobile *mob) {
    item *potion = ((item*)mob)->contents->item;
    add_constituents(lvl->chemistry[mob->x][mob->y], potion->chemistry);
    inventory_item *inv = ((item*)mob)->contents;
    ((item*)mob)->contents = inv->next;
    free((void*)inv);
    destroy_item(potion);
}

void mob_rotate_inventory(mobile* mob) {
    if (((item*)mob)->contents != NULL && ((item*)mob)->contents->next != NULL) {
        inventory_item *itm = ((item*)mob)->contents;
        while (itm->next->next != NULL) itm = itm->next;
        inventory_item *last = itm->next;
        itm->next = NULL;
        last->next = ((item*)mob)->contents;
        ((item*)mob)->contents = last;
    }
}

bool mob_quaff(mobile* mob) {
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

