#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


/*  This should be the only function to use from crop.c
 *  It takes an image and the four points of the grid and gives a new image
 *  the image in input in freed
 */
SDL_Surface *crop(SDL_Surface *image, double x1, double y1,
    double x2, double y2, double x3, double y3, double x4, double y4);

