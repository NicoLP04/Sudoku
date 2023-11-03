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
	SDL_UnlockSurface(surface);
	return (Uint8)(sum/len);
	
}

int* gethistogram(SDL_Surface* surface,int* tab)
{
	Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;

	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for(int i=0; i<len; ++i)
	{
		SDL_Color rgb1;
		SDL_GetRGB(pixels[i],surface->format,&rgb1.r,&rgb1.g,&rgb1.b) ;
		tab[rgb1.r]++;
	}
		

	SDL_UnlockSurface(surface);
	return tab;
}