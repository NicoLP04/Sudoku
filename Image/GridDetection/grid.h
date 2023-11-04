#ifndef GRID_H
#define GRID_H

#include "houghtransform.h"
#include "linked_list.h"

typedef struct Point
{
    int X;
    int Y;
} Point;

typedef struct Square
{
    Line top;
    Line bot;
    Line left;
    Line right;
} Square;

// functions that reduces the number of line received by the hough transform
List* reduceLines(List* lines);

#endif
