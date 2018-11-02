#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include "level.h"
#include "../log.h"
#include "../mob/mob.h"
#include "../los/los.h"

static bool step_towards(level *lvl, mobile *actor, int target_x, int target_y) {
    int new_x = actor->x;
    int new_y = actor->y;

    //TODO this doesn't exist anymore
    //step_towards(&new_x, &new_y, target_x, target_y, false);

    return(move_if_valid(lvl, actor, new_x, new_y));
}

static bool one_step(level *lvl, int from_x, int from_y, int to_x, int to_y, int *x, int *y) {
    int dx = to_x - from_x;
    int dy = to_y - from_y;

    int x_step = (dx >= 0) ? 1 : -1;
    int y_step = (dy >= 0) ? 1 : -1;

    int preferred_x, preferred_y;
    int fallback_x, fallback_y;

    // if the direction is 45 degrees, pick at random
    if (dx == dy) {
        // faking (dx,dy) values to feed the selector below
        if (rand() % 2 == 0) {
            dx = 1;
            dy = -1;
        } else {
            dy = 1;
            dx = -1;
        }
    }

    if (dx > dy) {
        preferred_x = from_x + x_step;
        preferred_y = from_y;

        fallback_x = from_x;
        fallback_y = from_y + y_step;
    } else {
        preferred_x = from_x;
        preferred_y = from_y + y_step;

        fallback_x = from_x + x_step;
        fallback_y = from_y;
    }

    if (is_position_valid(lvl, preferred_x, preferred_y)) {
        *x = preferred_x;
        *y = preferred_y;
        return true;
    } else if (is_position_valid(lvl, fallback_x, fallback_y)) {
        *x = fallback_x;
        *y = fallback_y;
        return true;
    } else {
        *x = from_x;
        *y = from_y;
        return false;
    }
}

void minotaur_fire(void *context, void* vmob) {
    mobile *mob = (mobile*)vmob;
    level *lvl = (level*)context;
    if (can_see(lvl, mob, lvl->player->x, lvl->player->y)) {
        step_towards(lvl, mob, lvl->player->x, lvl->player->y);
        ((item*) mob)->display = '>';
    } else {
        random_walk_fire(context, vmob);
    }
}

void umber_hulk_fire(void *context, void* vmob) {
    mobile *mob = (mobile*)vmob;
    if (rand()/(float)RAND_MAX > 0.8) {
        if (*(bool*)mob->state) {
            *(bool*)mob->state = false;
            mob->base.display = ICON_UMBER_HULK_ASLEEP;
        } else {
            *(bool*)mob->state = true;
            mob->base.display = ICON_UMBER_HULK_AWAKE;
        }
    }

    if (*(bool*)mob->state) {
        random_walk_fire(context, vmob);
    }
}

bool umber_hulk_invalidation(void *vmob) {
    mobile *mob = (mobile*)vmob;
    *(bool*)mob->state = true;
    mob->base.display = ICON_UMBER_HULK_AWAKE;
    return true;
}

int umber_hulk_next_firing(void *context, void* vmob, struct event_listener *listeners) {
    mobile *mob = (mobile*)vmob;
    if (*(bool*)mob->state) {
        float rate = 0.5;
        float r = ((float)rand()) / RAND_MAX;
        int next_fire = log(1-r)/(-rate) * TICKS_PER_TURN;
        if (next_fire < TICKS_PER_TURN) return TICKS_PER_TURN;
        return next_fire;
    } else {
        listeners[DAMAGE].handler = umber_hulk_invalidation;
        return INT_MAX;
    }
}

static void partition(level *lvl);

level* make_level(void) {
    level *lvl = malloc(sizeof *lvl);
    int level_width = MAX_MAP_WIDTH;
    int level_height = MAX_MAP_HEIGHT;

    lvl->active = true;
    lvl->width = level_width;
    lvl->height = level_height;
    lvl->keyboard_x = lvl->keyboard_y = 0;
    lvl->mob_count = 1 + 20;
    lvl->mobs = malloc(lvl->mob_count * (sizeof(mobile*)));
    for (int i=0; i < lvl->mob_count; i++) lvl->mobs[i] = make_mob(lvl);
    lvl->tiles = malloc(level_width * sizeof(unsigned char*));
    lvl->tiles[0] = malloc(level_height * level_width * sizeof(unsigned char));
    for (int i = 1; i < level_width; i++)
        lvl->tiles[i] = lvl->tiles[0] + i * level_height;

    lvl->memory = malloc(level_width * sizeof(unsigned char*));
    lvl->memory[0] = malloc(level_height * level_width * sizeof(unsigned char));
    for (int i = 1; i < level_width; i++)
        lvl->memory[i] = lvl->memory[0] + i * level_height;

    lvl->items = malloc(level_width * sizeof(inventory_item**));
    lvl->items[0] = malloc(level_height * level_width * sizeof(inventory_item*));
    for (int i = 1; i < level_width; i++)
        lvl->items[i] = lvl->items[0] + i * level_height;
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        lvl->items[x][y] = NULL;
    }

    lvl->chemistry = malloc(level_width * sizeof(inventory_item**));
    lvl->chemistry[0] = malloc(level_height * level_width * sizeof(inventory_item*));
    for(int i = 1; i < level_width; i++)
        lvl->chemistry[i] = lvl->chemistry[0] + i * level_height;
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        lvl->memory[x][y] = TILE_UNSEEN;
        lvl->chemistry[x][y] = make_constituents();
        lvl->chemistry[x][y]->elements[air] = 20;
    }

    lvl->chem_sys = make_default_chemical_system();

    lvl->sim = make_simulation((void*)lvl);

    partition(lvl);

    lvl->player = lvl->mobs[lvl->mob_count-1];
    lvl->player->x = lvl->player->y = 1;
    struct agent a;

    a.next_firing = every_turn_firing;
    a.fire = player_move_fire;
    a.state = (void*)lvl->player;
    a.listeners = ((item*)lvl->player)->listeners;
    simulation_push_agent(lvl->sim, &a);

    ((item*)lvl->player)->health = 10;
    ((item*)lvl->player)->display = ICON_HUMAN;
    ((item*)lvl->player)->name = malloc(sizeof(char)*9);
    strcpy(((item*)lvl->player)->name, "yourself");
    lvl->player->active = true;

    item* potion = malloc(sizeof(item)); // FIXME leaks
    potion->display = '!';
    potion->health = 1;
    potion->chemistry = make_constituents();
    potion->chemistry->elements[phosphorus] = 30;
    potion->name = "Phosphorous Potion";
    potion->type = Potion;
    push_inventory(lvl->player, potion);

    item* poison = malloc(sizeof(item)); // FIXME leaks
    poison->display = '!';
    poison->health = 1;
    poison->chemistry = make_constituents();
    poison->chemistry->elements[venom] = 30;
    poison->name = "Poison";
    poison->type = Potion;
    push_inventory(lvl->player, poison);

    item* antidote = malloc(sizeof(item)); // FIXME leaks
    antidote->display = '!';
    antidote->health = 1;
    antidote->chemistry = make_constituents();
    antidote->chemistry->elements[banz] = 30;
    antidote->name = "Antidote";
    antidote->type = Potion;
    push_inventory(lvl->player, antidote);

    item* stick = malloc(sizeof(item)); // FIXME leaks
    stick->display = '_';
    stick->health = 5;
    stick->chemistry = make_constituents();
    stick->chemistry->elements[wood] = 30;
    stick->name = "Stick";
    stick->type = Weapon;
    push_inventory(lvl->player, stick);

    for (int i=0; i < lvl->mob_count-1; i++) {
        lvl->mobs[i]->x = rand()%(level_width-2) + 1;
        lvl->mobs[i]->y = rand()%(level_height-2) + 1;
        lvl->mobs[i]->active = true;

        switch (rand()%4) {
            case 0:
                ((item*)lvl->mobs[i])->display = ICON_GOBLIN;
                lvl->mobs[i]->stacks = true;
                ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*7);
                a.next_firing = random_walk_next_firing;
                a.fire = random_walk_fire;
                a.state = (void*)lvl->mobs[i];
                a.listeners = ((item*)lvl->mobs[i])->listeners;
                simulation_push_agent(lvl->sim, &a);
                strcpy(((item*)lvl->mobs[i])->name, "goblin");
                break;
            case 1:
                ((item*)lvl->mobs[i])->display = ICON_ORC;
                ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*4);
                a.next_firing = random_walk_next_firing;
                a.fire = random_walk_fire;
                a.state = (void*)lvl->mobs[i];
                a.listeners = ((item*)lvl->mobs[i])->listeners;
                simulation_push_agent(lvl->sim, &a);
                strcpy(((item*)lvl->mobs[i])->name, "orc");
                break;
            case 2:
                ((item*)lvl->mobs[i])->display = ICON_UMBER_HULK_AWAKE;
                ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*4);
                ((item*)lvl->mobs[i])->health = 30;
                lvl->mobs[i]->state = malloc(sizeof(bool));
                *(bool*)lvl->mobs[i]->state = true;
                a.next_firing = umber_hulk_next_firing;
                a.fire = umber_hulk_fire;
                a.state = (void*)lvl->mobs[i];
                a.listeners = ((item*)lvl->mobs[i])->listeners;
                simulation_push_agent(lvl->sim, &a);
                strcpy(((item*)lvl->mobs[i])->name, "umber hulk");
                break;
            default:
                ((item*)lvl->mobs[i])->display = ICON_MINOTAUR;
                ((item*)lvl->mobs[i])->name = malloc(sizeof(char)*9);
                a.next_firing = random_walk_next_firing;
                a.fire = minotaur_fire;
                a.state = (void*)lvl->mobs[i];
                a.listeners = ((item*)lvl->mobs[i])->listeners;
                simulation_push_agent(lvl->sim, &a);
                strcpy(((item*)lvl->mobs[i])->name, "minotaur");
                break;
        }
    }
    for (int i = 0; i < lvl->mob_count; i++) {
        schedule_event(lvl->sim, (struct agent*)vector_get(lvl->sim->agents, i), 0);
    }

    return lvl;
}

void destroy_level(level *lvl) {
    free((void *)lvl->tiles[0]);
    free((void *)lvl->tiles);
    free((void *)lvl->memory[0]);
    free((void *)lvl->memory);
    free((void *)lvl->items[0]);
    free((void *)lvl->items);
    for (int x = 0; x < lvl->width; x++) for (int y = 0; y < lvl->height; y++) {
        destroy_constituents(lvl->chemistry[x][y]);
    }
    free((void *)lvl->chemistry[0]);
    free((void *)lvl->chemistry);
    destroy_chemical_system(lvl->chem_sys);
    for (int i = 0; i < lvl->mob_count; i++) destroy_mob(lvl->mobs[i]);
    free((void *)lvl->mobs);
    destroy_simulation(lvl->sim);
    free((void *)lvl);
}

int rec_partition(int **room_map, int x, int y, int w, int h, int rm) {
    // disabled partitioning
    if (w*h > 10*10 && rand()%100 < 0) {
        int hw = w/2;
        int hh = h/2;
        int max_rm, new_rm;

        max_rm = rec_partition(room_map, x, y, hw, hh, rm);
        new_rm = rec_partition(room_map, x + hw, y, w-hw, hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x + hw, y + hh, w-hw, h-hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = rec_partition(room_map, x, y + hh, hw, h-hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        return max_rm;
    } else {
        rm += 1;
        for (int xx = x; xx < x+w; xx++) {
            for (int yy = y; yy < y+h; yy++) {
                room_map[xx][yy] = rm;
            }
        }
        return rm;
    }
}

static void partition(level *lvl) {
    int **partitioning = malloc(lvl->width * sizeof(int*));
    for (int i = 0; i != lvl->width; i++) partitioning[i] = malloc(lvl->height*sizeof(int));
    int **potential_doors= malloc(lvl->width * sizeof(int*));
    for (int i = 0; i != lvl->width; i++) potential_doors[i] = malloc(lvl->height*sizeof(int));

    int rooms = rec_partition(partitioning, 0, 0, lvl->width, lvl->height, 0);

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            lvl->tiles[x][y] = TILE_FLOOR;
        }
    }

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            for (int dx = -1; dx < 1; ++dx) for (int dy = -1; dy < 1; ++dy) {
                int xx = x+dx;
                int yy = y+dy;
                if (xx < 0 || yy < 0 || xx >= lvl->width -1 || yy >= lvl->height -1) {
                    lvl->tiles[x][y] = TILE_WALL;
                } else if (partitioning[xx][yy] != partitioning[x][y]) {
                    lvl->tiles[x][y] = TILE_WALL;
                    if (abs(dx+dy) == 1) {
                        potential_doors[x][y] = true;
                    }
                }
            }
        }
    }

    bool room_accessible[rooms];
    for (int i = 0; i < rooms; i++) room_accessible[i] = false;
    room_accessible[partitioning[0][0]] = true;
    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            if (potential_doors[x][y]) {
                bool door_needed = false;
                bool door_possible = false;
                int rm_a, rm_b;
                if (x+1 < lvl->width && x-1 >= 0 && lvl->tiles[x+1][y] != TILE_WALL && lvl->tiles[x-1][y] != TILE_WALL) {
                    rm_a = partitioning[x+1][y];
                    rm_b = partitioning[x-1][y];
                    door_possible = true;
                }
                if (y+1 < lvl->height && y-1 >= 0 && lvl->tiles[x][y+1] != TILE_WALL && lvl->tiles[x][y-1] != TILE_WALL) {
                    rm_a = partitioning[x][y+1];
                    rm_b = partitioning[x][y-1];
                    door_possible = true;
                }
                if (door_possible && rand()%100 > 0) {
                    if (room_accessible[rm_a] + !room_accessible[rm_b] != 1) {
                        door_needed = true;
                        room_accessible[rm_b]=true;
                        room_accessible[rm_a]=true;
                    }
                }
                if (door_needed) {
                    lvl->tiles[x][y] = DOOR_OPEN;
                } else {
                    lvl->tiles[x][y] = TILE_WALL;
                }
            }
        }
    }

    free((void *)partitioning[0]);
    free((void *)partitioning);
    free((void *)potential_doors[0]);
    free((void *)potential_doors);
}

void level_push_item(level *lvl, item *itm, int x, int y) {
    inventory_item *new_inv = malloc(sizeof(inventory_item));
    new_inv->next = NULL;
    new_inv->item = itm;
    if (lvl->items[x][y] == NULL) {
        lvl->items[x][y] = new_inv;
    } else {
        inventory_item *inv = lvl->items[x][y];
        while (inv->next != NULL) inv = inv->next;
        inv->next = new_inv;
    }
}

item* level_pop_item(level *lvl, int x, int y) {
    if (lvl->items[x][y] == NULL) {
        return NULL;
    } else {
        inventory_item *old = lvl->items[x][y];
        lvl->items[x][y] = old->next;
        item *itm = old-> item;
        free(old);
        return itm;
    }
}

bool is_position_valid(level *lvl, int x, int y) {
    if (x >= lvl->width || x < 0) {
        logger("ERROR %s: %s: %d\n", "is_position_valid", "x is out of bounds", x);
        return false;
    } else if (y >= lvl->height || y < 0) {
        logger("ERROR %s: %s: %d\n", "is_position_valid", "y is out of bounds", y);
        return false;
    } else if (lvl->tiles[x][y] == TILE_WALL || lvl->tiles[x][y] == DOOR_CLOSED) {
        // from a performance standpoint, this should be the first test
        return false;
    } else {
        for (int i=0; i < lvl->mob_count; i++) {
            if (lvl->mobs[i]->active && !lvl->mobs[i]->stacks && lvl->mobs[i]->x == x && lvl->mobs[i]->y == y) {
                return false;
            }
        }
    }
    return true;
}

bool move_if_valid(level *lvl, mobile *mob, int x, int y) {
    if (is_position_valid(lvl, x, y)) {
        mob->x = x;
        mob->y = y;
        return true;
    } else {
        return false;
    }
}
