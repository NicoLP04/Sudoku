#define _GNU_SOURCE

#include <stdio.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "filters.h"
#include "average.h"

SDL_Surface* load_image(const char* path)
{
	SDL_Surface* temp = IMG_Load(path);
	if (temp == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_Surface* ret = SDL_ConvertSurfaceFormat(temp,
			SDL_PIXELFORMAT_RGB888, 0);
	if (ret == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_FreeSurface(temp);

	return ret;

}



int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
    {
        errx(EXIT_FAILURE, "Usage: image-file");
    }

    // - Create a surface from the colored image.
	SDL_Surface* s = load_image(argv[1]);
	if (s == NULL)
    {
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    }
    //  - Transform the surface into grayscale
    //printf("****Applying Grayscale*****\n") ;
	surface_to_grayscale(s);

    Uint8 seuil = getaverage(s);

    int histo[256] = {0};
    int* histogram = gethistogram(s,histo);
    //printf("****Applying Contrast*****\n") ;
    surface_to_contrast(s,histogram);
    //printf("****Applying Median*****\n") ;
    surface_to_median(s);
    //printf("****Applying Threshold*****\n") ;
    printf("%d\n",seuil);
    if (seuil<150)
    {
        surface_to_threshold(s,seuil+20);
    }

    else if (seuil <175)
    {
        surface_to_threshold(s,seuil-(255-seuil));
    }
    else if (seuil <190)
    {
        surface_to_threshold(s,seuil-50);
    }
    else if (seuil<210)
    {
        surface_to_threshold(s,seuil-15);
    }
    else
    {
        surface_to_threshold(s,123);
    }

    //surface_to_invert(s);
    // - Save the image

    surface_to_median(s);
    //surface_to_smooth(s); Issue on smooth
    //surface_to_median(s);
    //printf("****Applying Invert*****\n") ;
    surface_to_invert(s);
   // printf("****Saving ....*****\n") ;
	char num = 0;

	int i;

	for (i = strlen(argv[1]) - 1; i >= 0 && argv[1][i] != '.'; --i)
	;

	num = argv[1][i - 1];

	char* filename = NULL;

	asprintf(&filename, "image_%c.jpeg", num);

    SDL_SaveBMP(s,filename);
	free(filename);

    // - Free the surface.
    SDL_FreeSurface(s);


    return EXIT_SUCCESS;
}
