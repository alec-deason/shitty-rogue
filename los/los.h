#ifndef INC_LOS_H
#define INC_LOS_H

//TODO do we need this?
//#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../level/level.h"
#include "../log.h"

typedef bool (*checker_func)(level *lvl, int x, int y);

bool line_of_sight(level *lvl, int a_x, int a_y, int b_x, int b_y);
bool can_see(level *lvl, mobile *actor, int target_x, int target_y);
bool braise(level *lvl, int a_x, int a_y, int b_x, int b_y, checker_func checker);

#endif
