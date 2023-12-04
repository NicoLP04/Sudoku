#ifndef IMAGE_H
#define IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void save_texture(const char* file_name, SDL_Renderer* renderer,
        SDL_Texture* texture);


SDL_Surface* load_image(const char* path);


#endif