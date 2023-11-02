#include "filters.h"

static Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);

	Uint8 average = 0.3*r + 0.59*g + 0.11*b;

	return SDL_MapRGB(format, average, average, average);
}

void surface_to_grayscale(SDL_Surface* surface)
{
	Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;

	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for(int i=0; i<len; ++i)
		pixels[i] = pixel_to_grayscale(pixels[i], format);

	SDL_UnlockSurface(surface);
}