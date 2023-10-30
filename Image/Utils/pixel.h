#ifndef PIXEL_H
#define PIXEL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>

// return the pixel at specified coordinates x,y
Uint32 get_pixel(SDL_Surface* s, unsigned int x, unsigned int y);

// put the pixel at specified coordinates x,y
void put_pixel(SDL_Surface* s, unsigned int x, unsigned int y);

#endif
