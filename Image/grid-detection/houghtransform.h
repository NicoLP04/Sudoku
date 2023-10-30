#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


// allocate memory for a matrix of size x by y
unsigned int** initMat(unsigned int x, unsigned int y);

// convert degrees to radian
double deg2rad(double degrees);

// convert radian to degrees
double rad2deg(double radian);

// the houghtransform algorithm
void houghtransform(SDL_Surface* image, SDL_Renderer* draw_image);

// creates a SDL surface from a path
SDL_Surface* load_image(const char* path);

#endif
