#include "filters.h"

// ***************      GRAYSCALE    *****************

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

// ***************      THRESHOLDING    *****************

static Uint32 pixel_to_threshold(Uint32 pixel_color, Uint8 threshold,
		SDL_PixelFormat* format)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);

	Uint8 average = (r + g + b)/3;
    if (average<threshold)
    {
        average=0;
    }
    else
    {
        average =255;
    }
	return SDL_MapRGB(format, average, average, average);
}

void surface_to_threshold(SDL_Surface* surface, Uint8 threshold)
{
    Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;

	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for(int i=0; i<len; ++i)
		pixels[i] = pixel_to_threshold(pixels[i],threshold, format);

	SDL_UnlockSurface(surface);
}


// ***************      INVERT    *****************
static Uint32 pixel_to_invert(Uint32 pixel_color,SDL_PixelFormat* format)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);

	return SDL_MapRGB(format, 255-r, 255-g, 255-b);
}

void surface_to_invert(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;

	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for(int i=0; i<len; ++i)
		pixels[i] = pixel_to_invert(pixels[i], format);

	SDL_UnlockSurface(surface);
}


// ***************      MEDIAN    *****************

void surface_to_median(SDL_Surface* surface)
{
	Uint32* pixels = surface ->pixels;
	size_t w = surface-> w ;
	size_t h = surface->h ;

	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	for (size_t i = 1 ; i <h-1;i++)
	{
		for (size_t j = 1 ; j<w -1;j++)
		{
			Uint32 mat[] = {pixels[(i-1)*w+j-1],pixels[(i-1)*w+j],
				pixels[(i-1)*w+j+1],pixels[i*w+j-1],pixels[i*w+j],
				pixels[i*w+j+1],pixels[(i+1)*w+j-1],pixels[(i+1)*w+j],
				pixels[(i+1)*w+j+1]};
			for (size_t k=0; k<8;k++)
			{
				for(size_t l = k+1;l<9;l++)
				{
					SDL_Color rgb1;
					SDL_Color rgb2;
					SDL_GetRGB(mat[k],surface->format,&rgb1.r,&rgb1.g,&rgb1.b);
					SDL_GetRGB(mat[l],surface->format,&rgb2.r,&rgb2.g,&rgb2.b);
					if(rgb1.r>rgb2.r)
					{
						Uint32 temp = mat[k];
						mat[k]=mat[l];
						mat[l]=temp;
					}
				}
			}
			pixels[i*w+j]=mat[4];
		}
	}
}



// ***************      CONTRAST    *****************

static Uint32 pixel_to_contrast(Uint32 pixel_color, SDL_PixelFormat* format,
		int* histogram,int len)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);

	double temp = (double)histogram[r]/(double)len;

	double val = r*(259*(temp+255))/(255*(259-temp));

	return SDL_MapRGB(format, val, val, val);
}

void surface_to_contrast(SDL_Surface* surface,int* histogram)
{
    Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;
	int err = SDL_LockSurface(surface);
	if(err!=0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	for(int i=0; i<len; ++i)
		pixels[i] = pixel_to_contrast(pixels[i], format,histogram,len);

	SDL_UnlockSurface(surface);
}
