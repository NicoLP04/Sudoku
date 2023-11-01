#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdlib.h>


SDL_Surface* extract(SDL_Surface *image)
{
	Uint32* imagePixels = image->pixels;    

	size_t width = image->w;
	size_t height = image->h;

	size_t xstart = 0;
	size_t ystart = 0;
	size_t xend = width - 1;
	size_t yend = height - 1;
	Uint8 l = 100;

	int stop = 0;
	while (stop == 0)
	{
		if (xstart >= width)
			return image;
		for (size_t y = 0; y < height; y++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[y * width + xstart], image->format, &r, &g, &b);
			if (r <= l && g <= l && b <= l)
				stop = 1;
		}
		xstart++;
	}
	xstart-=2;
	
	stop = 0;
	while (stop == 0)
	{
		for (size_t y = 0; y < height; y++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[y * width + xend], image->format, &r, &g, &b);
			if (r <= l && g <= l && b <= l)
				stop = 1;
		}
		xend--;
	}
	xend+=3;
	
	stop = 0;
	while (stop == 0)
	{
		for (size_t x = 0; x < width; x++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[ystart * width + x], image->format, &r, &g, &b);
			if (r <= l && g <= l && b <= l)
				stop = 1;
		}
		ystart++;
	}
	ystart-=2;

	stop = 0;
	while (stop == 0)
	{
		for (size_t x = 0; x < width; x++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[yend * width + x], image->format, &r, &g, &b);
			if (r <= l && g <= l && b <= l)
				stop = 1;
		}
		yend--;
	}
	yend+=3;

	SDL_Surface *cell = SDL_CreateRGBSurface(0, xend - xstart, yend - ystart, 32,0,0,0,0);
	Uint32* cellPixels = cell->pixels;

	for (size_t x = xstart; x < xend; x++)
	{	
		for (size_t y = ystart; y < yend; y++)
        	{
         		cellPixels[(y - ystart) * (xend - xstart) + x - xstart] = imagePixels[y * width + x];
		}
	}


	SDL_FreeSurface(image);

	return cell;
}


SDL_Surface* remove_border(SDL_Surface *image)
{

	Uint32* imagePixels = image->pixels;    

	size_t width = image->w;
	size_t height = image->h;

	double w10 = width / 10;
	double h10 = height / 10;
	size_t cellw = width - 2 * w10;
	size_t cellh = height - 2 * h10;

	SDL_Surface *cell = SDL_CreateRGBSurface(0, cellw, cellh, 32,0,0,0,0);
	Uint32* cellPixels = cell->pixels;

	for (size_t x = width / 10; x < width - w10; x++)
	{	
		for (size_t y = height / 10; y < height - h10; y++)
        	{
         		cellPixels[(y - (height / 10)) * cellw + x - width / 10] = imagePixels[y * width + x];
		}
	}

	SDL_FreeSurface(image);

	return cell;
}



void split(SDL_Surface *image)
{
    Uint32* imagePixels = image->pixels;    

    size_t width = image->w;
    size_t height = image->h;

    size_t xincrem = width / 9;
    size_t yincrem = height / 9;
    double xadd = 0;
    double yadd = 0;

    size_t numCell = 1;
    for (size_t y = 0; y < height; y += yincrem)
    {
        for (size_t x = 0; x < width; x += xincrem)
        {
            if (y + yincrem <= height && x + xincrem <= width)
            {
		SDL_Surface *cell = SDL_CreateRGBSurface(0, xincrem, yincrem, 32,0,0,0,0);
		Uint32* cellPixels = cell->pixels;

		// copy pixels
		for (size_t a = x; a < x + xincrem; a++)
    		{
     			for (size_t b = y; b < y + yincrem; b++)
        		{
         			cellPixels[(b - y) * xincrem + a - x] = imagePixels[b * width + a];
        		}
    		}

		cell = remove_border(cell);
		cell = extract(cell);

		// save cell
		char *cellName = malloc(20 * sizeof(char));
		cellName[0] = 0;
		strcat(cellName, "Cells/");

		char num[256];
		snprintf(num, sizeof(num), "%zu", numCell);
		strcat(cellName, num);
		
		strcat(cellName, ".png");

		IMG_SavePNG(cell, cellName);

		free(cellName);
		SDL_FreeSurface(cell);
		numCell++;
            }

	    xadd += ((double)(width % 9) / 9);
	    if (xadd >= 1)
	    {
		    x++;
		    xadd--;
	    }
        }
	yadd += ((double)(height % 9) / 9);
	if (yadd >= 1)
	{
		y++;
		yadd--;
	}
    }
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp=IMG_Load(path);
    if (temp  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* newsurf = SDL_ConvertSurfaceFormat(temp,SDL_PIXELFORMAT_RGB888,0);
    if (newsurf  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(temp);
    return newsurf;
}

int main(int argc, char** argv)
{
	if (argc != 2)
		errx(EXIT_FAILURE, "Usage: image-file");

	SDL_Surface *image = load_image(argv[1]);

	split(image);

	SDL_FreeSurface(image);

	return EXIT_SUCCESS;
}
