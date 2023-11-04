#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <err.h>

SDL_Surface* resizeImage(SDL_Surface* originalSurface,
int newWidth, int newHeight)
{
    if (originalSurface == NULL)
        return NULL;
    SDL_Surface* resizedSurface = SDL_CreateRGBSurfaceWithFormat(0, newWidth,
    newHeight, originalSurface->format->BitsPerPixel,
    originalSurface->format->format);

    if (resizedSurface == NULL)
    {
        printf("Unable to create resized surface! SDL Error: %s\n",
        SDL_GetError());
        return NULL;
    }
    if (SDL_BlitScaled(originalSurface, NULL, resizedSurface, NULL) != 0)
    {
        printf("Unable to scale image! SDL Error: %s\n", SDL_GetError());
        return NULL;
    }
    return resizedSurface;
}

int main(int argc, char* args[])
{
    if (argc != 4)
    {
        errx(EXIT_FAILURE, "Usage: %s image-file / width / height",  args[0]);
    }
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	    errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Initialize SDL_image for image loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	    errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* originalSurface = IMG_Load(args[1]);
    if (originalSurface == NULL)
    {
        errx(EXIT_FAILURE,
        "Unable to load image from '%s'! SDL_image Error: %s\n",
        args[1], IMG_GetError());
    }
    int newWidth = atoi(args[2]);
    int newHeight = atoi(args[3]);
    SDL_Surface* resizedSurface =
    resizeImage(originalSurface, newWidth, newHeight);
    if (resizedSurface != NULL)
    {
        IMG_SavePNG(resizedSurface, "resized.png");
        SDL_FreeSurface(originalSurface);
        SDL_FreeSurface(resizedSurface);
    }
    IMG_Quit();
    SDL_Quit();
    return 0;
}
