#ifndef FILTERS_H
#define FILTERS_H

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

void surface_to_grayscale(SDL_Surface* surface);

void surface_to_seuillage(SDL_Surface* surface,Uint8 seuil);

void surface_to_invert(SDL_Surface* surface);

void surface_to_median(SDL_Surface* surface);

void surface_to_smooth(SDL_Surface* surface);

#endif
