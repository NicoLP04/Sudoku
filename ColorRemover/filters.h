#ifndef FILTERS_H
#define FILTERS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

// return the pixel at specified coordinates x,y
void surface_to_grayscale(SDL_Surface* surface);

#endif
