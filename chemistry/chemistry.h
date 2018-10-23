#ifndef INC_CHEMESTRY_H
#define INC_CHEMESTRY_H
#include <stdbool.h>

enum element_names {
    fire,
    earth,
    water,
    air,
    wood,
    phosphorus,
    foogle,
    banz,
    life,
    venom,
    ash
};
#define ELEMENT_COUNT ash+1

typedef struct constituents {
    int elements[ELEMENT_COUNT];
    bool stable;
} constituents;


typedef struct reaction {
    constituents input;
    constituents output;
} reaction;

typedef struct chemical_system {
    reaction* reactions;
    int num_reactions;
    bool is_volatile[ELEMENT_COUNT];
} chemical_system;

constituents* make_constituents();
void destroy_constituents(constituents *con);
void add_constituents(constituents *dest, constituents *src);

chemical_system* make_default_chemical_system();
chemical_system* make_chemical_system(int num_reactions);
void destroy_chemical_system(chemical_system* sys);

void react(chemical_system *system, constituents *input, constituents *context);
bool apply_reaction(reaction *re, constituents *input, constituents *context);

#endif
