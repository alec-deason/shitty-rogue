#include <ncurses.h>

#include "game.h"
#include "input.h"
#include "mob/actions.h"

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
    //TODO Don't advance turn on any key press.
    //Have explicit "wait" key (e.g. <SPACE>)
    char *inventory;
    char *message = malloc(sizeof(char)*MESSAGE_LENGTH);
    int input = getch();
    int return_code = 0;

    switch (input) {
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
        case 'H':
            snprintf(message, MESSAGE_LENGTH, "HELP: (i)nv (d)rop (.)pickup (r)otate inv (q)uaff (v)smash (e)xamine (s)tatus (t)ile (Q)uit");
            print_message(message);
            break;
        case 'i':
            inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
            snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
            print_message(message);
            free((void*)inventory);
            break;
        case 'd':
            mob_drop_item(lvl, lvl->player);
            break;
        case '.':
            mob_pickup_item(lvl, lvl->player);
            break;
        case 'r':
            mob_rotate_inventory(lvl->player);
            inventory = inventory_string(lvl->player, MESSAGE_LENGTH);
            snprintf(message, MESSAGE_LENGTH, "Your inventory contains: %s", inventory);
            print_message(message);
            free((void*)inventory);
            break;
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
        case 'o':
            mob_toggle_door(lvl, lvl->player, get_direction());
            break;
        case 'Q':
            return_code = -1;
    }
    free((void*)message);
    return return_code;
}
