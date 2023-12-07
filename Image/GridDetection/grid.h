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

// function that retuns a list of struct Square from the list of Lines
List findAllSquares(List* lines, int w, int h, char *filename);

// returns the best square to represent the sudoku grid from the square list
Square FindBestSquare(List* squarelist);

#endif