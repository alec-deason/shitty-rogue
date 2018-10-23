#ifndef MOB_ACTIONS_H
#define MOB_ACTIONS_H

#include "../level/level.h"
#include "mob.h"
#include "../game.h"

void mob_rotate_inventory(mobile* mob);
bool mob_quaff(mobile* mob);
void mob_drop_item(level *lvl, mobile *mob);
void mob_toggle_door(level *lvl, mobile *mob, direction dir);
void mob_pickup_item(level *lvl, mobile *mob);
void mob_smash_potion(level *lvl, mobile *mob);

#endif
