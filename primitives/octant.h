#ifndef OCTANT_H
#define OCTANT_H

#endif // OCTANT_H

#include <stdlib.h>
#include <math.h>
#include "gfx_basics.h"

#define OCTANT_RAD ((2 * M_PI) / 8)

int determine_octant(Line line);
void convert(Point& p, int o);
void revert(Point& p, int o);

// Helpers
void swap(Point& p);
void negate(Point& p);
