#include "../curses/color.h"

// Tiles
#define TILE_WALL '#'
#define TILE_FLOOR '.' | LIGHT_GRAY
#define TILE_NOT_VISIBLE ' '

// Scenery
#define DOOR_OPEN '-' | BROWN
#define DOOR_CLOSED '+'
#define DOOR_LOCKED DOOR_CLOSED

// Effects
#define STATUS_BURNING '*' | RED

// Mobs
#define ICON_HUMAN '@'
#define ICON_PLAYER ICON_HUMAN | YELLOW
#define ICON_GOBLIN 'o' | GREEN
#define ICON_ORC 'O'
#define ICON_UMBER_HULK_AWAKE 'U'
#define ICON_UMBER_HULK_ASLEEP 'z'
#define ICON_MINOTAUR 'M'
#define ICON_MINOTAUR_CHARGING 'M' | RED

// Items
#define ICON_ASH '~'
#define ICON_POTION '!'
#define ICON_STICK '_'

// Emotes
#define EMOTE_ANGRY '!' | RED

// Other

// Initialization value only and should never be seen
// May render as a black question mark on a white
// lozenge, or an upside down question mark.
#define ICON_UNDEFINED '\277'
