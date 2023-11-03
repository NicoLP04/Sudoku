#ifndef AVERAGE_H
#define AVERAGE_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

Uint8 getaverage(SDL_Surface* surface);

int* gethistogram(SDL_Surface* surface,int* tab);

#endif