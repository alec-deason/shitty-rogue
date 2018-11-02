#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "../log.h"
#include "../renderer.h"
#include "color.h"
#include "../input.h"


char message_banner[MESSAGE_LENGTH];

void init_rendering_system(void) {
    initscr();

    if(!init_colors()) {
        //TODO print TERM environment variable
        logger("Terminal does not support color.\n");
        exit(1);
    }

    raw();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    curs_set(0);

    int row,col;
    getmaxyx(stdscr,row,col);
    logger("Terminal size: %d %d\n", row, col);
    mvprintw(0,0, "c");

}

void cleanup_rendering_system(void) {
    endwin();
}


//TODO When the same message (e.g. quaff) is repeated, it
//should be clear somehow that there were multiple messages
void print_message(char *msg) {
    strncpy(message_banner, msg, MESSAGE_LENGTH);
}

static void draw_mobile(mobile *mob, int x_offset, int y_offset) {
    char icon = ((item*)mob)->display;

    if (mob->emote) {
        icon = mob->emote;
        mob->emote = false;
    }

    mvprintw(mob->y + y_offset, mob->x + x_offset, "%c", icon);
}

void draw_level(level *lvl) {
    int row,col;
    getmaxyx(stdscr,row,col);
    row -= 1;

    // Offset to keep player in center
    int x_offset = col / 2 - lvl->player->x;
    int y_offset = row / 2 - lvl->player->y;

    // Draw map and items
    for (int xx = 0; xx < col; xx++) {
        for (int yy = 0; yy < row; yy++) {
            // (xx,yy) are screen coordinates
            // (x ,y ) are map coordinates
            int x = xx - x_offset;
            int y = yy - y_offset;

            char icon = TILE_UNSEEN;

            if ((0 <= x && x < lvl->width) && (0 <= y && y < lvl->height)) {
                //TODO wrapper function with clear name
                if (can_see(lvl, lvl->player, x, y)) {
                    if (lvl->chemistry[x][y]->elements[fire] > 0) {
                        icon = STATUS_BURNING;
                        attron(COLOR_PAIR(RED));
                    } else if (lvl->items[x][y] != NULL) {
                        icon = lvl->items[x][y]->item->display;
                    } else {
                        icon = lvl->tiles[x][y];
                    }
                }
                // Fog of war
                if (icon == TILE_UNSEEN) {
                    icon = lvl->memory[x][y];
                    attron(COLOR_PAIR(YELLOW));
                } else {
                    lvl->memory[x][y] = icon;
                }
            }
            mvprintw(yy, xx, "%c", icon);
            //TODO this should be a general unsetter, not this
            attroff(COLOR_PAIR(YELLOW));
            attroff(COLOR_PAIR(RED));
        }
    }

    // Draw mobs
    for (int i=0; i < lvl->mob_count; i++) {
        mobile* mob = lvl->mobs[i];
        if (mob->active && can_see(lvl, lvl->player, mob->x, mob->y)) {
            if ((0 < mob->y + y_offset && mob->y + y_offset <= row) && (0 < mob->x + x_offset && mob->x + x_offset <= col)) {
                draw_mobile(mob, x_offset, y_offset);
            }
        }
    }
    draw_mobile(lvl->player, x_offset, y_offset);
    move(row, 0);
    clrtoeol();
    mvprintw(row, 0, message_banner);
}

int get_keystroke(void) {
    return getch();
}
