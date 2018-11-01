#include "los.h"

bool line_of_sight(level *lvl, int a_x, int a_y, int b_x, int b_y) {
    return braise(lvl, a_x, a_y, b_x, b_y, &is_position_valid);
}

bool can_see(level *lvl, mobile *actor, int target_x, int target_y) {
    // This is between a thing and a position
    // It just wraps line_of_sight for easier English reading
    // Making a thing-to-thing function seems too specific
    return line_of_sight(lvl, actor->x, actor->y, target_x, target_y);
}

bool braise(level *lvl, int a_x, int a_y, int b_x, int b_y, checker_func checker) {
    // initialize starting (x,y)
    int x = a_x;
    int y = a_y;

    // calculate x- and y-distances
    int dx = abs(b_x - a_x);
    int dy = abs(b_y - a_y);

    // set signs on increments
    int x_increment = (b_x >= a_x) ? 1 : -1;
    int y_increment = (b_y >= a_y) ? 1 : -1;

    // pointers to things what get changed
    int *rise, *run;
    int *stepper, *step;
    int *bumper, *bump;

    //logger("=== Initialized ===\na=(%d,%d) b=(%d,%d) dxy=(%d + %d,%d + %d)\n",a_x,a_y,b_x,b_y,dx,x_increment,dy,y_increment);

    // set up which values will be modified based on
    // the slope of the line
    if (dx > dy) {
        // The slope is shallow
        rise = &dy;
        run = &dx;

        stepper = &x;
        step = &x_increment;

        bumper = &y;
        bump = &y_increment;
    } else {
        // The slope is steep (or 45 degrees)
        rise = &dx;
        run = &dy;

        stepper = &y;
        step = &y_increment;

        bumper = &x;
        bump = &x_increment;
    }

    // multiply everything by 2 to get rid of the 0.5 and
    // leave only integer math (and shifts)
    int error = 0;

    // error adjustements
    int increment = *rise;
    int drain = *run;

    int i; // so we can use it after the loop

    // step 'run' many steps along the stepper axis
    for (i = 0; i < *run; i++) {
        //logger("Step #%d: (%d,%d) step: %d + %d bump: %d + %d error: %d + %d - %d\n",i+1,x,y,*stepper,*step,*bumper,*bump,error,increment,drain);
        // handle horizontal and vertical exception cases
        // (This could be way more optimized)
        if (dx == 0) {
            // vertical line
            y += y_increment;
        } else if (dy == 0) {
            // horizontal line
            x += x_increment;
        } else {
            //printf("Step %d\n", i+1);
            // advance stepper
            *stepper += *step;
            // advance "error"
            error += increment;
    
            // bucket is full
            if (error >= *run) {
                // advance the bumper
                *bumper += *bump;
                // "reset" the "error"
                error -= drain;
            }
        }


        if (x == b_x && y == b_y) {
            // We made it. Doesn't matter if the final square
            // was inaccessible, because most of the time it'll
            // be occupied by something.
            //logger("Made it to (%d,%d)\n", x, y);
            return true;
        }
    
        /* check for valid space here */
        if (! checker(lvl, x, y)) {
            //logger("Failed checker() on step #%d out of %d at (%d,%d)\n", i + 1, *run, x, y);
            return false;
        }
    }
}
