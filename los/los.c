#include "los.h"

//TODO make a mob-to-mob LOS function
//TODO make a player-can-see function

bool line_of_sight(level *lvl, int origin_x, int origin_y, int target_x, int target_y) {
    //TODO how the hell is is_position_valid working in here?
    //TODO because I included level.h? yuck!
    return check_line(lvl, origin_x, origin_y, target_x, target_y, &is_position_valid);
}

bool can_see(level *lvl, mobile *actor, int target_x, int target_y) {
    return line_of_sight(lvl, actor->x, actor->y, target_x, target_y);
}

static bool check_line(level *lvl, int origin_x, int origin_y, int target_x, int target_y, checker_func checker) {
    int cur_x = origin_x;
    int cur_y = origin_y;

    int dx = abs(target_x - origin_x);
    int dy = abs(target_y - origin_y);

    int x_increment = (target_x >= origin_x) ? 1 : -1;
    int y_increment = (target_y >= origin_y) ? 1 : -1;

    int *rise, *run;
    int *stepper, *step;
    int *bumper, *bump;

    //TODO Should 45 degrees be shallow or steep?
    if (dx > dy) {
        // shallow slope - step along X-axis
        rise = &dy;
        run = &dx;

        stepper = &cur_x;
        step = &x_increment;

        bumper = &cur_y;
        bump = &y_increment;
    } else {
        // steep slope - step along Y-axis
        rise = &dx;
        run = &dy;

        stepper = &cur_y;
        step = &y_increment;

        bumper = &cur_x;
        bump = &x_increment;
    }

    int bump_debt = 0;

    while (true) {
        if (dx == 0) {
            // exception case - vertical line
            cur_y += y_increment;
        } else if (dy == 0) {
            // exception case - horizontal line
            cur_x += x_increment;
        } else {
            *stepper += *step;
            bump_debt += *rise;

            if (bump_debt >= *run) {
                *bumper += *bump;
                bump_debt -= *run;
            }
        }

        if (cur_x == target_x && cur_y == target_y) {
            return true;
        }

        if (! checker(lvl, cur_x, cur_y)) {
            return false;
        }
    }
}
