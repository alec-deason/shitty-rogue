#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include "level.h"
#include "../log.h"
#include "../mob/mob.h"
#include "../los/los.h"

static bool one_step(level *lvl, int *from_x, int *from_y, int to_x, int to_y) {
    int dx = to_x - *from_x;
    int dy = to_y - *from_y;

    int x_step, y_step;

    if (dx > 0) {
        x_step = 1;
    } else if (dx < 0) {
        x_step = -1;
    } else {
        x_step = 0;
    }

    if (dy > 0) {
        y_step = 1;
    } else if (dy < 0) {
        y_step = -1;
    } else {
        y_step = 0;
    }

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
        preferred_x = *from_x + x_step;
        preferred_y = *from_y;

        fallback_x = *from_x;
        fallback_y = *from_y + y_step;
    } else {
        preferred_x = *from_x;
        preferred_y = *from_y + y_step;

        fallback_x = *from_x + x_step;
        fallback_y = *from_y;
    }

    if (is_position_valid(lvl, preferred_x, preferred_y)) {
        *from_x = preferred_x;
        *from_y = preferred_y;
        return true;
    } else if (is_position_valid(lvl, fallback_x, fallback_y)) {
        *from_x = fallback_x;
        *from_y = fallback_y;
        return true;
    } else {
        return false;
    }
}

static void minotaur_fire(void *context, void* vmob) {
    mobile *mob = (mobile*)vmob;
    level *lvl = (level*)context;

    if (can_see(lvl, mob, lvl->player->x, lvl->player->y)) {
        if (one_step(lvl, &mob->x, &mob->y, lvl->player->x, lvl->player->y)) {
            ((item*) mob)->display = ICON_MINOTAUR_CHARGING;
        } else {
            ((item*) mob)->display = EMOTE_ANGRY;
        }
    } else {
        random_walk_fire(context, vmob);
    }
}

static void umber_hulk_fire(void *context, void* vmob) {
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

static bool umber_hulk_invalidation(void *vmob) {
    mobile *mob = (mobile*)vmob;
    *(bool*)mob->state = true;
    mob->base.display = ICON_UMBER_HULK_AWAKE;
    return true;
}

static int umber_hulk_next_firing(void *context, void* vmob, struct event_listener *listeners) {
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

static void make_map(level *lvl);

level* make_level(void) {
    level *lvl = malloc(sizeof *lvl);
    int level_width = MAX_MAP_WIDTH;
    int level_height = MAX_MAP_HEIGHT;

    lvl->active = true;
    lvl->width = level_width;
    lvl->height = level_height;
    lvl->keyboard_x = lvl->keyboard_y = 0;
    lvl->mob_count = 1 + NUM_MONSTERS;
    lvl->mobs = malloc(lvl->mob_count * (sizeof(mobile*)));
    for (int i=0; i < lvl->mob_count; i++) lvl->mobs[i] = make_mob(lvl);
    lvl->tiles = malloc(level_width * sizeof(int*));
    lvl->tiles[0] = malloc(level_height * level_width * sizeof(int));
    for (int i = 1; i < level_width; i++)
        lvl->tiles[i] = lvl->tiles[0] + i * level_height;

    lvl->memory = malloc(level_width * sizeof(int*));
    lvl->memory[0] = malloc(level_height * level_width * sizeof(int));
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

    make_map(lvl);

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
    potion->display = ICON_POTION;
    potion->health = 1;
    potion->chemistry = make_constituents();
    potion->chemistry->elements[phosphorus] = 30;
    potion->name = "Phosphorous Potion";
    potion->type = Potion;
    push_inventory(lvl->player, potion);

    item* poison = malloc(sizeof(item)); // FIXME leaks
    poison->display = ICON_POTION;
    poison->health = 1;
    poison->chemistry = make_constituents();
    poison->chemistry->elements[venom] = 30;
    poison->name = "Poison";
    poison->type = Potion;
    push_inventory(lvl->player, poison);

    item* antidote = malloc(sizeof(item)); // FIXME leaks
    antidote->display = ICON_POTION;
    antidote->health = 1;
    antidote->chemistry = make_constituents();
    antidote->chemistry->elements[banz] = 30;
    antidote->name = "Antidote";
    antidote->type = Potion;
    push_inventory(lvl->player, antidote);

    item* stick = malloc(sizeof(item)); // FIXME leaks
    stick->display = ICON_STICK;
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

        //TODO magic number
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

static int partition(int **room_map, int x, int y, int w, int h, int rm) {
    if (w*h > 10*10 && rand()%100 < PARTITIONING_PROBABILITY * 100) {
        int hw = w/2;
        int hh = h/2;
        int max_rm, new_rm;

        max_rm = partition(room_map, x, y, hw, hh, rm);
        new_rm = partition(room_map, x + hw, y, w-hw, hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = partition(room_map, x + hw, y + hh, w-hw, h-hh, max_rm);
        if (new_rm > max_rm) max_rm = new_rm;
        new_rm = partition(room_map, x, y + hh, hw, h-hh, max_rm);
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

static void make_map(level *lvl) {
    int **room_tiles = malloc(lvl->width * sizeof(int*));
    for (int i = 0; i != lvl->width; i++) {
        room_tiles[i] = malloc(lvl->height*sizeof(int));
    }

    int **potential_doors= malloc(lvl->width * sizeof(int*));
    for (int i = 0; i != lvl->width; i++) {
        potential_doors[i] = malloc(lvl->height*sizeof(int));
    }

    int max_room_id = partition(room_tiles, 0, 0, lvl->width, lvl->height, 0);

    // initialize all tiles to bare floor
    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            lvl->tiles[x][y] = TILE_FLOOR;
        }
    }

    for (int x = 0; x < lvl->width; x++) {
        for (int y = 0; y < lvl->height; y++) {
            // for all nine squares around the tile
            for (int dx = -1; dx < 1; dx++) {
                for (int dy = -1; dy < 1; dy++) {
                    int xx = x+dx;
                    int yy = y+dy;
                    // if it's the level border, it's a wall
                    if (xx < 0 || yy < 0 || xx >= lvl->width -1 || yy >= lvl->height -1) {
                        lvl->tiles[x][y] = TILE_WALL;
                    // if it's a room boundary (i.e. room ID changes), it's a wall
                    } else if (room_tiles[xx][yy] != room_tiles[x][y]) {
                        lvl->tiles[x][y] = TILE_WALL;
                        // doors can only be N, E, S, or W of us
                        if (abs(dx+dy) == 1) {
                            potential_doors[x][y] = true;
                        }
                    }
                }
            }
        }
    }

    // whether each room is accessible from the "root" room
    bool room_connected[max_room_id];

    for (int i = 0; i < max_room_id; i++) {
        room_connected[i] = false;
    }

    // the "root" room
    int rand_x = (rand() % (lvl->width - 1)) + 1;
    int rand_y = (rand() % (lvl->height - 1)) + 1;

    room_connected[room_tiles[rand_x][rand_y]] = true;

    // We are building a tree of connected rooms (via door placement)
    // starting at a "root" room. Potential doors only become doors if
    // they would connected an already-connected room to an unconnected
    // room.
    for (int x = 1; x < lvl->width - 1; x++) {
        for (int y = 1; y < lvl->height - 1; y++) {
            if (potential_doors[x][y]) {
                bool door_possible = false;
                int rm_a, rm_b;

                if (x+1 < lvl->width && x-1 >= 0 && lvl->tiles[x+1][y] != TILE_WALL && lvl->tiles[x-1][y] != TILE_WALL) {
                // a "horizontal" door
                    rm_a = room_tiles[x+1][y];
                    rm_b = room_tiles[x-1][y];
                    door_possible = true;
                } else if (y+1 < lvl->height && y-1 >= 0 && lvl->tiles[x][y+1] != TILE_WALL && lvl->tiles[x][y-1] != TILE_WALL) {
                // a "vertical" door
                    rm_a = room_tiles[x][y+1];
                    rm_b = room_tiles[x][y-1];
                    door_possible = true;
                }

                if (door_possible && (rand()%100 <= DOOR_PROBABILITY * 100) && (room_connected[rm_a] + !room_connected[rm_b] != 1)) { // XOR
                    logger("Placing door at (%d,%d)\n", x, y);
                    lvl->tiles[x][y] = DOOR_OPEN;
                    room_connected[rm_b]=true;
                    room_connected[rm_a]=true;
                }
            }
        }
    }

    free((void *)room_tiles[0]);
    free((void *)room_tiles);
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
