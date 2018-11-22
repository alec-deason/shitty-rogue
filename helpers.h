#ifndef HELPERS_H
#define HELPERS_H

//TODO rename this file - macros.h ?
//TODO put into dir structure somewhere

#define frand() ((float)rand() / RAND_MAX)

#define prob(p) (frand() <= p)

#define rand_int(n) (rand() % (n + 1))

#endif
