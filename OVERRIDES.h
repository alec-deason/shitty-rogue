// Overrides for constants in one place

// Couldn't figure out how to cleanly use -D to
// set this in the Makefile
//#define OVERRIDES

#ifdef OVERRIDES

// Disable level partitioning
#undef PARTITIONING_PERCENTAGE
#define PARTITIONING_PERCENTAGE 0

// Limit number of monsters
#undef NUM_MONSTERS
#define NUM_MONSTERS 1

#endif
