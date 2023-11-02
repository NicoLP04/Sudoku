#ifndef PIXEL_H
#define PIXEL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

// return the pixel at specified coordinates x,y
Uint32 get_pixel(SDL_Surface* s, unsigned int x, unsigned int y);

#endif
