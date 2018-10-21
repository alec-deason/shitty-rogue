#include <ncurses.h>

#include "game.h"
#include "log.h"
#include "input.h"
#include "mob/actions.h"

struct command {
    int key;
    char *name;
    bool requires_direction;
    int (*handler)(level *lvl, int key, direction dir);
};

static int advance_turn(level *lvl, int key, direction dir) {
    return 0;
}

static int exit_game(level *lvl, int key, direction dir) {
    return -1;
}

static int toggle_door(level *lvl, int key, direction dir) {
     mob_toggle_door(lvl, lvl->player, dir);
     return 0;
}

static int print_help(level *lvl, int key, direction dir);

static int print_inventory(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    char *inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
    snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
    print_message(message);
    free((void*)inventory);
    return 0;
}

static int rotate_inventory(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    mob_rotate_inventory(lvl->player);
    char *inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
    snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
    print_message(message);
    free((void*)inventory);
    return 0;
}

static int drop_item(level *lvl, int key, direction dir) {
    mob_drop_item(lvl, lvl->player);
    return 0;
}

static int pickup_item(level *lvl, int key, direction dir) {
    mob_pickup_item(lvl, lvl->player);
    return 0;
}

static int move_player(level *lvl, int key, direction dir) {
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
    return 0;
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
    { .key = '.', .name="Pickup Item", .requires_direction = false, .handler = pickup_item}
};

static int NUM_COMMANDS = 12;

static int print_help(level *lvl, int key, direction dir) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    int total = 0;

    total += snprintf(message, MESSAGE_LENGTH, "HELP:");
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (total >= MESSAGE_LENGTH) break;
        total += snprintf(message+total, MESSAGE_LENGTH-total, " (%c)%s", COMMANDS[i].key, COMMANDS[i].name);
    }
    print_message(message);
    return 0;
}

void print_location_elements(level *lvl, mobile *mob) {
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    constituents *chem = lvl->chemistry[mob->x][mob->y];
    snprintf(message, MESSAGE_LENGTH, "wood: %d air: %d fire: %d", chem->elements[wood], chem->elements[air], chem->elements[fire]);
    print_message(message);
    free((void*)message);
}

direction get_direction(void) {
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

int get_input(level *lvl) {
    int key = getch();
    direction dir = no_direction;

    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (key == COMMANDS[i].key) {
            if (COMMANDS[i].requires_direction) {
                dir = get_direction();
            }
            logger("Player command: %s\n", COMMANDS[i].name);
            return COMMANDS[i].handler(lvl, key, dir);
        }
    }
    return 0;
}

int get_input_old(level *lvl) {
    //TODO Don't advance turn on any key press.
    //Have explicit "wait" key (e.g. <SPACE>)
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    int input = getch();
    int return_code = 0;

    switch (input) {
        case 'q':
            if(mob_quaff(lvl->player)) {
                print_message("You drink a potion.");
            } else {
                print_message("That isn't a potion.");
            }
            break;
        case 'v':
            if (((item*)lvl->player)->contents != NULL && ((item*)lvl->player)->contents->item->type == Potion) {
                mob_smash(lvl, lvl->player);
                print_message("You smash the potion on the floor.");
            } else print_message("That isn't a potion.");
            break;
        case 'e':
            if (((item*)lvl->player)->contents != NULL) {
                snprintf(message, MESSAGE_LENGTH, "wood: %d fire: %d ash: %d",
                        ((item*)lvl->player)->contents->item->chemistry->elements[wood],
                        ((item*)lvl->player)->contents->item->chemistry->elements[fire],
                        ((item*)lvl->player)->contents->item->chemistry->elements[ash]
                        );
                print_message(message);
            }
            break;
        case 's':
            snprintf(message, MESSAGE_LENGTH, "You have %d hit points. venom: %d banz: %d life: %d", ((item*)lvl->player)->health, ((item*)lvl->player)->chemistry->elements[venom], ((item*)lvl->player)->chemistry->elements[banz], ((item*)lvl->player)->chemistry->elements[life]);
            print_message(message);
            break;
        case 't':
            print_location_elements(lvl, lvl->player);
            break;
    }
    free((void*)message);
    return return_code;
}
