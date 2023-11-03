#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <err.h>

SDL_Surface* resizeImage(SDL_Surface* originalSurface, int newWidth, int newHeight) {
    if (originalSurface == NULL) {
        return NULL;
    }

    // Create a new surface with the desired width and height
    SDL_Surface* resizedSurface = SDL_CreateRGBSurfaceWithFormat(0, newWidth, newHeight, originalSurface->format->BitsPerPixel, originalSurface->format->format);
    if (resizedSurface == NULL) {
        printf("Unable to create resized surface! SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    // Use SDL's scaling function to resize the image
    if (SDL_BlitScaled(originalSurface, NULL, resizedSurface, NULL) != 0) {
        printf("Unable to scale image! SDL Error: %s\n", SDL_GetError());
        return NULL;
    }

    return resizedSurface;
}

int main(int argc, char* args[]) {
    if (argc != 4) {
        printf("Usage: %s <image_file> <width> <height>\n", args[0]);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image for image loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    // Load the original image
    SDL_Surface* originalSurface = IMG_Load(args[1]);
    if (originalSurface == NULL) {
        printf("Unable to load image from '%s'! SDL_image Error: %s\n", args[1], IMG_GetError());
        return 1;
    }

    // Get the desired width and height from command-line arguments
    int newWidth = atoi(args[2]);
    int newHeight = atoi(args[3]);

    // Resize the image
    SDL_Surface* resizedSurface = resizeImage(originalSurface, newWidth, newHeight);

    if (resizedSurface != NULL) {
        // Save or display the resized image
        IMG_SavePNG(resizedSurface, "resized.png");
        
        // Clean up resources
        SDL_FreeSurface(originalSurface);
        SDL_FreeSurface(resizedSurface);
    }

    // Quit SDL and SDL_image
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}
