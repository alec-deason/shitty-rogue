#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "chemistry.h"

constituents* make_constituents() {
    constituents *con = malloc(sizeof(constituents));
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        con->elements[i] = 0;
    }
    con->stable = true;
    return con;
}

void destroy_constituents(constituents* con) {
    free((void*)con);
}

chemical_system* make_default_chemical_system() {
    chemical_system *sys = make_chemical_system(10);

    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < ash; i++) {
            sys->reactions[j].input.elements[i] = -1;
            sys->reactions[j].output.elements[i] = -1;
        }
    }

    for (int i = 0; i < ELEMENT_COUNT; i++) sys->is_volatile[i] = false;
    sys->is_volatile[fire] = true;
    sys->is_volatile[air] = true;

    // Burning
    sys->reactions[1].input.elements[wood] = 5;
    sys->reactions[1].input.elements[fire] = 1;
    sys->reactions[1].input.elements[air] = 10;
    sys->reactions[1].output.elements[wood] = 0;
    sys->reactions[1].output.elements[fire] = 4;
    sys->reactions[1].output.elements[air] = 0;
    sys->reactions[1].output.elements[ash] = 5;

    //Fire dying
    sys->reactions[2].input.elements[fire] = 1;
    sys->reactions[2].output.elements[fire] = 0;

    //Phosphorous combustion
    sys->reactions[3].input.elements[phosphorus] = 1;
    sys->reactions[3].input.elements[air] = 1;
    sys->reactions[3].output.elements[phosphorus] = 0;
    sys->reactions[3].output.elements[air] = 0;
    sys->reactions[3].output.elements[fire] = 10;

    // Anti-venom
    sys->reactions[0].input.elements[venom] = 10;
    sys->reactions[0].input.elements[banz] = 10;
    sys->reactions[0].output.elements[venom] = 0;
    sys->reactions[0].output.elements[banz] = 0;
    sys->reactions[0].output.elements[ash] = 10;

    return sys;
}

chemical_system* make_chemical_system(int num_reactions) {
    chemical_system *sys = malloc(sizeof(chemical_system));
    sys->reactions = malloc(num_reactions*sizeof(reaction));
    sys->num_reactions = num_reactions;
    return sys;
}

void destroy_chemical_system(chemical_system* sys) {
    free((void*)sys->reactions);
    free((void*)sys);
}

bool reaction_possible(reaction *re, constituents *input, constituents *ctx) {
    for (int i=0; i < ELEMENT_COUNT; i++) {
        int avaliable = input->elements[i];
        if (ctx != NULL) avaliable += ctx->elements[i];
        if (re->input.elements[i] > avaliable) {
            return false;
        }
    }
    return true;
}

void react(chemical_system *sys, constituents *input, constituents *context) {
    bool did_react = false;
    int r = rand();
    for (int i = 0; i < sys->num_reactions; i++) {
        int j = (i+r)%sys->num_reactions;
        reaction *re = &sys->reactions[j];
        if (apply_reaction(re, input, context)) {
            did_react = true;
            break;
        }
    }
    input->stable = !did_react;
}

bool apply_reaction(reaction *re, constituents *input, constituents *ctx) {
    if (reaction_possible(re, input, ctx)) {
        for (int i=0; i < ELEMENT_COUNT; i++) {
            int needed = re->input.elements[i];
            float proportion_from_input = 1.0;
            if (needed > 0) {
                if (needed > input->elements[i]) {
                    proportion_from_input = input->elements[i]/(float)needed;
                    needed -= input->elements[i];
                    input->elements[i] = 0;
                    ctx->elements[i] -= needed;
                } else {
                    input->elements[i] -= needed;
                }
            }
            if (re->output.elements[i] > 0) {
                if (proportion_from_input != 1.0) {
                    int to_input = round(proportion_from_input*re->output.elements[i]);
                    int to_output = re->output.elements[i] - to_input;
                    input->elements[i] += to_input;
                    ctx->elements[i] += to_output;
                } else {
                    input->elements[i] += re->output.elements[i];
                }
            }
        }
        return true;
    }
    return false;
}

void add_constituents(constituents *dest, constituents *src) {
        for (int i=0; i < ELEMENT_COUNT; i++) {
            dest->elements[i] += src->elements[i];
        }
}
