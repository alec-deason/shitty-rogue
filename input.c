#include <ncurses.h>

#include "game.h"
#include "log.h"
#include "input.h"
#include "mob/actions.h"

struct command {
    int key;
    char *name;
    bool requires_direction;
    void (*handler)(level *lvl, int key, direction dir);
};

static void advance_turn(level *lvl, int key, direction dir) {
}

static void exit_game(level *lvl, int key, direction dir) {
    lvl->active = false;
}

static void toggle_door(level *lvl, int key, direction dir) {
     mob_toggle_door(lvl, lvl->player, dir);
}

static void print_help(level *lvl, int key, direction dir);

static void print_inventory(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    char *inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
    snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
    print_message(message);
    free((void*)inventory);
}

static void rotate_inventory(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    mob_rotate_inventory(lvl->player);
    char *inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
    snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
    print_message(message);
    free((void*)inventory);
}

static void drop_item(level *lvl, int key, direction dir) {
    mob_drop_item(lvl, lvl->player);
}

static void pickup_item(level *lvl, int key, direction dir) {
    mob_pickup_item(lvl, lvl->player);
}

static void move_player(level *lvl, int key, direction dir) {
    switch (key) {
        case KEY_UP:
            lvl->keyboard_y = -1;
            break;
        case KEY_DOWN:
            lvl->keyboard_y = 1;
            break;
        case KEY_RIGHT:
            lvl->keyboard_x = 1;
            break;
        case KEY_LEFT:
            lvl->keyboard_x = -1;
            break;
    }
}

static void quaff(level *lvl, int key, direction dir) {
    if(mob_quaff(lvl->player)) {
        print_message("You drink a potion.");
    } else {
        print_message("That isn't a potion.");
    }
}


static void smash_potion(level *lvl, int key, direction dir) {
    if (((item*)lvl->player)->contents != NULL && ((item*)lvl->player)->contents->item->type == Potion) {
        mob_smash_potion(lvl, lvl->player);
        print_message("You smash the potion on the floor.");
    } else print_message("That isn't a potion.");
}

static void examine_item(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    if (((item*)lvl->player)->contents != NULL) {
        snprintf(message, MESSAGE_LENGTH, "wood: %d fire: %d ash: %d",
                ((item*)lvl->player)->contents->item->chemistry->elements[wood],
                ((item*)lvl->player)->contents->item->chemistry->elements[fire],
                ((item*)lvl->player)->contents->item->chemistry->elements[ash]
                );
        print_message(message);
    }
    free((void*)message);
}

static void status(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    snprintf(message, MESSAGE_LENGTH, "You have %d hit points. venom: %d banz: %d life: %d", ((item*)lvl->player)->health, ((item*)lvl->player)->chemistry->elements[venom], ((item*)lvl->player)->chemistry->elements[banz], ((item*)lvl->player)->chemistry->elements[life]);
    print_message(message);
    free((void*)message);
}

static void examine_location(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    constituents *chem = lvl->chemistry[lvl->player->x][lvl->player->y];
    snprintf(message, MESSAGE_LENGTH, "wood: %d air: %d fire: %d", chem->elements[wood], chem->elements[air], chem->elements[fire]);
    print_message(message);
    free((void*)message);
}

struct command COMMANDS[] = {
    { .key = ' ', .name="Wait", .requires_direction = false, .handler = advance_turn},
    { .key = KEY_UP, .name="Move up", .requires_direction = false, .handler = move_player},
    { .key = KEY_RIGHT, .name="Move right", .requires_direction = false, .handler = move_player},
    { .key = KEY_DOWN, .name="Move down", .requires_direction = false, .handler = move_player},
    { .key = KEY_LEFT, .name="Move left", .requires_direction = false, .handler = move_player},
    { .key = 'o', .name="Toggle door", .requires_direction = true, .handler = toggle_door},
    { .key = 'H', .name="Help", .requires_direction = false, .handler = print_help},
    { .key = 'Q', .name="Quit", .requires_direction = false, .handler = exit_game},
    { .key = 'i', .name="Inventory", .requires_direction = false, .handler = print_inventory},
    { .key = 'r', .name="Rotate Inventory", .requires_direction = false, .handler = rotate_inventory},
    { .key = 'd', .name="Drop Item", .requires_direction = false, .handler = drop_item},
    { .key = '.', .name="Pickup Item", .requires_direction = false, .handler = pickup_item},
    { .key = 'q', .name="Quaff Potion", .requires_direction = false, .handler = quaff},
    { .key = 'v', .name="Smash Potion", .requires_direction = false, .handler = smash_potion},
    { .key = 'e', .name="Examine Item", .requires_direction = false, .handler = examine_item},
    { .key = 's', .name="Status", .requires_direction = false, .handler = status},
    { .key = 't', .name="Examine Location", .requires_direction = false, .handler = examine_location}
};

static int NUM_COMMANDS = 17;

static void print_help(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    int total = 0;

    total += snprintf(message, MESSAGE_LENGTH, "HELP:");
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (total >= MESSAGE_LENGTH) break;
        total += snprintf(message+total, MESSAGE_LENGTH-total, " (%c)%s", COMMANDS[i].key, COMMANDS[i].name);
    }
    print_message(message);
}

static direction get_direction(void) {
    switch (getch()) {
        case KEY_UP:
            return up;
        case KEY_RIGHT:
            return right;
        case KEY_DOWN:
            return down;
        case KEY_LEFT:
            return left;
        default:
            return no_direction;
    }
}

void get_input(level *lvl) {
    int key = getch();
    direction dir = no_direction;

    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (key == COMMANDS[i].key) {
            if (COMMANDS[i].requires_direction) {
                dir = get_direction();
            }
            logger("Player command: %s\n", COMMANDS[i].name);
            COMMANDS[i].handler(lvl, key, dir);
        }
    }
}
