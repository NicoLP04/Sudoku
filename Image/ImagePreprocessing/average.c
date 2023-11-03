#include "average.h"

static int getpixelsum(Uint32 pixel_color,SDL_PixelFormat* format)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);

	int average = (int)(r + g + b)/3;

	return average;
}

Uint8 getaverage(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;

	int err = SDL_LockSurface(surface);
	int sum = 0;
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for(int i=0; i<len; ++i)
		sum += getpixelsum(pixels[i],format) ;

	return (Uint8)(sum/len);
	SDL_UnlockSurface(surface);
}