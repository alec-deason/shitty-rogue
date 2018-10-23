#include <ncurses.h>
#include "color.h"

bool init_colors() {
    // use can_change_color() instead?
    if (! has_colors()) {
        // Terminal doesn't support colors
        return false;
    }

    // Need to grab default background color or it's grey
    use_default_colors();

    start_color();

    // Colors: black red green yellow blue magenta cyan white
    //
    // Matching these indices to the tput color indices
    init_pair(DEFAULT, DEFAULT, DEFAULT);

    init_pair(BLACK, COLOR_BLACK, DEFAULT);
    init_pair(RED, COLOR_RED, DEFAULT);
    init_pair(GREEN, COLOR_GREEN, DEFAULT);
    init_pair(YELLOW, COLOR_YELLOW, DEFAULT);
    init_pair(BLUE, COLOR_BLUE, DEFAULT);
    init_pair(MAGENTA, COLOR_MAGENTA, DEFAULT);
    init_pair(CYAN, COLOR_CYAN, DEFAULT);
    init_pair(WHITE, COLOR_WHITE, DEFAULT);

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
