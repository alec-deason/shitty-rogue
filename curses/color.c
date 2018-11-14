#include <ncurses.h>
#include <stdlib.h>
#include "color.h"
#include "../log.h"

bool init_colors() {
    // use can_change_color() instead?
    if (! has_colors()) {
        logger("Error: Terminal doesn't have colors: %s\n", getenv("TERM"));
        return false;
    }

    if (! can_change_color()) {
        logger("Error: Terminal can't change colors: %s\n", getenv("TERM"));
        return false;
    }

    // Need to grab default background color or it's grey
    use_default_colors();

    start_color();

    //TODO check that enough colors are supported?
    logger("Terminal %s is supporting %d colors and %d color pairs\n", getenv("TERM"), COLORS, COLOR_PAIRS);

    // Colors: black red green yellow blue magenta cyan white
    //
    // Matching these indices to the tput color indices
    init_pair(ORIG_COLORS, ORIG_COLORS, ORIG_COLORS);

    init_pair(PAIR_BLACK, COLOR_BLACK, ORIG_COLORS);
    init_pair(PAIR_RED, COLOR_RED, ORIG_COLORS);
    init_pair(PAIR_GREEN, COLOR_GREEN, ORIG_COLORS);
    init_pair(PAIR_YELLOW, COLOR_YELLOW, ORIG_COLORS);
    init_pair(PAIR_BLUE, COLOR_BLUE, ORIG_COLORS);
    init_pair(PAIR_MAGENTA, COLOR_MAGENTA, ORIG_COLORS);
    init_pair(PAIR_CYAN, COLOR_CYAN, ORIG_COLORS);
    init_pair(PAIR_WHITE, COLOR_WHITE, ORIG_COLORS);

    // Custom colors

    // init_color(short ID, short RED, short GREEN, short BLUE)
    //  ID <= COLORS
    //  RED, GREEN, BLUE < 1000

    init_color(COLOR_BROWN, 600, 300, 0);
    init_pair(PAIR_BROWN, COLOR_BROWN, ORIG_COLORS);

    return true;
}

/*
        A_NORMAL        Normal display (no highlight)
        A_STANDOUT      Best highlighting mode of the terminal.
        A_UNDERLINE     Underlining
        A_REVERSE       Reverse video
        A_BLINK         Blinking
        A_DIM           Half bright
        A_BOLD          Extra bright or bold
        A_PROTECT       Protected mode
        A_INVIS         Invisible or blank mode
        A_ALTCHARSET    Alternate character set
        A_CHARTEXT      Bit-mask to extract a character
        COLOR_PAIR(n)   Color-pair number n
*/
