#include "grid.h"
#include "houghtransform.h"
#include "image.h"
#include "linked_list.h"
#include "pixel.h"
#include "sobel.h"



// main function :
// does the full process of grid detection.
// step 1 apply sobel on the image
// step 2 calculate houghspace from sobel image
// step 3 reduce lines detected by houghtransform
// step 4 detect biggest square representing the grid
// step 5 re-order square corners coordinates
// step 6 crop the soduko grid
// FINAL STEP -> save image


int main(int argc, char** argv)
{
	// arguments parsing.
	if (argc != 2)
	{
		printf("Error: expected 1 argument, got: %i\n", argc - 1);
		printf("Usage: ./COMMAND <PATH>\n");
		return EXIT_FAILURE;
	}

	// -----------------------------------------------------------------------
	// setup the SDL environnement.
	// SDL initialisation.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	// window.
	SDL_Window* window = SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN);
	if (window == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	// renderer.
	SDL_Renderer* renderer =
		SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());
	// -----------------------------------------------------------------------



}
