#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "filters.h"

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


    // - Save the image
    SDL_SaveBMP(s,"grayimage.jpeg");

    // - Free the surface.
    SDL_FreeSurface(s);
	

    return EXIT_SUCCESS;
}