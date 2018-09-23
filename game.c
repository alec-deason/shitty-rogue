#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

#define WALL '#'
#define FLOOR '.'

#define ICON_HUMAN '@'
#define ICON_GOBLIN 'o'
#define ICON_ORC 'O'

#define OUCH '!'

#define LEVEL_WIDTH  200
#define LEVEL_HEIGHT 100

char level[LEVEL_HEIGHT][LEVEL_WIDTH];

enum Behavior {RandomWalk};

struct mobile {
    char display;
    int x;
    int y;
    bool active;
    enum Behavior behavior;
    char emote;
};

struct mobile player;

#define MAX_MOBS 100
struct mobile mobs[MAX_MOBS];

bool is_position_valid(int x, int y) {
    return level[y][x] != WALL;
}

bool move_if_valid(struct mobile *mob, int x, int y) {
    if (is_position_valid(x, y)) {
        mob->x = x;
        mob->y = y;
        return true;
    } else {
        return false;
    }
}

void draw_mobile(struct mobile *mob, int dx, int dy) {
    char display = mob->display;

    if (mob->emote) {
        display = mob->emote;
        mob->emote = false;
    }

    mvprintw(dy+mob->y, dx+mob->x, "%c", display);
}
void draw() {
    int row,col;
    getmaxyx(stdscr,row,col);

    int dx = col/2 - player.x;
    int dy = row/2 - player.y;

    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int xx = x - dx;
            int yy = y - dy;
            if (xx >= 0 && xx < LEVEL_WIDTH && yy >= 0 && yy < LEVEL_HEIGHT) {
                mvprintw(y, x, "%c", level[yy][xx]);
            } else {
                mvprintw(y, x, " ");
            }
        }
    }
    for (int i=0; i < MAX_MOBS; i++) {
        if (mobs[i].active) {
            draw_mobile(&mobs[i], dx, dy);
        }
    }
    draw_mobile(&player, dx, dy);
}

void setup_level() {
    for (int x = 0; x < LEVEL_WIDTH; x++) {
        for (int y = 0; y < LEVEL_HEIGHT; y++) {
            if (x == 0 || y == 0 || y == LEVEL_HEIGHT-1 || x == LEVEL_WIDTH-1) {
                level[y][x] = WALL;
            } else {
                level[y][x] = FLOOR;
            }
        }
    }
    player.x = player.y = 1;
    player.display = ICON_HUMAN;
    for (int i=0; i < MAX_MOBS; i++) {
        mobs[i].active = false;
    }

    for (int i=0; i < MAX_MOBS; i++) {
        mobs[i].x = rand()%(LEVEL_WIDTH-2) + 1;
        mobs[i].y = rand()%(LEVEL_HEIGHT-2) + 1;
        mobs[i].behavior = RandomWalk;
        mobs[i].active = true;

        if (rand()%2 == 0) {
            mobs[i].display = ICON_GOBLIN;
        } else {
            mobs[i].display = ICON_ORC;
        }
    }
}

int move_player() {
    int x = player.x;
    int y = player.y;
    switch (getch()) {
        case KEY_UP:
            y -= 1;
            break;
        case KEY_DOWN:
            y += 1;
            break;
        case KEY_RIGHT:
            x += 1;
            break;
        case KEY_LEFT:
            x -= 1;
            break;
        case 'q':
            return -1;
    }

    move_if_valid(&player, x, y);
    return 0;
}

void move_mobile(struct mobile *mob) {
    int x = mob->x;
    int y = mob->y;
    switch (mob->behavior) {
        case RandomWalk:
            if (rand()%2 == 0) {
                x += rand()%3 - 1;
            } else {
                y += rand()%3 - 1;
            }
    }
    if (!(move_if_valid(mob, x, y))) {
        mob->emote = OUCH;
    }
}

void main() {
        int ch;
        srand(time(NULL));
        initscr();
        raw();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        curs_set(0);

        setup_level();
        draw();
        while (move_player() == 0) {
            for (int i=0; i < MAX_MOBS; i++) {
                if (mobs[i].active) {
                    move_mobile(&mobs[i]);
                }
            }
            draw();
        }

        endwin();
}
