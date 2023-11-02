#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "filters.h"
// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale)
{
    SDL_Event event;
    SDL_Texture* t = colored;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
			case SDL_QUIT:
				return;

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					draw(renderer, t);
			break;

			case SDL_KEYDOWN:
				t = (t == grayscale) ? colored : grayscale;
				draw(renderer, t);
        }
    }
}

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
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

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Initialize the SDL.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a window.
	SDL_Window* window = SDL_CreateWindow("", 0, 0, 480, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a renderer.
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a surface from the colored image.
	SDL_Surface* s = load_image(argv[1]);
	if (s == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());


    // - Resize the window according to the size of the image.
	SDL_SetWindowSize(window,s->w,s->h);

    // - Create a texture from the colored surface.
	SDL_Texture* colored = SDL_CreateTextureFromSurface(renderer, s);
	if (colored == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Convert the surface into grayscale.
	surface_to_grayscale(s);

    // - Create a new texture from the grayscale surface.
	SDL_Texture* grayscale = SDL_CreateTextureFromSurface(renderer, s);
	if (grayscale == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Free the surface.
	SDL_FreeSurface(s);

    // - Dispatch the events.
	event_loop(renderer, colored, grayscale);

    // - Destroy the objects.
	SDL_DestroyTexture(colored);
	SDL_DestroyTexture(grayscale);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

    return EXIT_SUCCESS;
}