#ifndef SOBEL_H
#define SOBEL_H

#include <SDL2/SDL.h>

// Function to apply the Sobel filter to the image
void applySobel(SDL_Surface *input, SDL_Surface *output);

#endif // SOBEL_H