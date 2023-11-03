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

	SDL_Surface* ret = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
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
	surface_to_grayscale(s);
    
    Uint8 seuil = getaverage(s);
    
    printf("%d",seuil);
    
    if (seuil<150)
    {
        surface_to_seuillage(s,seuil+20);
    }
    
    else if (seuil <175)
    {
        surface_to_seuillage(s,seuil-(255-seuil));
    }
    else if (seuil <190)
    {
        surface_to_seuillage(s,seuil-50);
    }
    else if (seuil<210)
    {
        surface_to_seuillage(s,seuil-15);
    }
    else {surface_to_seuillage(s,123);}
    
    //surface_to_invert(s);
    // - Save the image
    
    surface_to_median(s);
    //surface_to_smooth(s); Issue on smooth
    //surface_to_median(s);
    surface_to_invert(s);
    SDL_SaveBMP(s,"image.jpeg");

    // - Free the surface.
    SDL_FreeSurface(s);
	

    return EXIT_SUCCESS;
}