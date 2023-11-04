#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

typedef struct Line
{
    int X0;
    int Y0;
    int X1;
    int Y1;
    double theta;
} Line;

// convert a line struct to a void pointer
void* line2voidptr(Line line);

// allocate memory for a matrix of size x by y
unsigned int** initMat(unsigned int x, unsigned int y);

// free the square matrix of size len
void freeMat(unsigned int** mat, double len);

// convert degrees to radian
double deg2rad(double degrees);

// convert radian to degrees
double rad2deg(double radian);

// fills the array arr of length len from minVal to maxVal incrementing
// the values with step
void array_fill(double* arr, double len, double step, double maxVal,
        double minVal);

// the houghtransform algorithm
List houghtransform(SDL_Surface* image, SDL_Renderer* draw_image);

#endif
