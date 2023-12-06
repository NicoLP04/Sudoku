#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

SDL_Surface* extract(SDL_Surface *image)
{
	Uint32* imagePixels = image->pixels;

	size_t width = image->w;
	size_t height = image->h;

	size_t xstart = width / 2;
	size_t ystart = height / 2;
	size_t xend = width - 1;
	size_t yend = height - 1;
	Uint8 l = 200;

  int cont = 1;
	while (cont == 1)
	{
		if (xend <= 0)
			return image;
		for (size_t y = 0; y < height; y++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[y * width + xend], image->format,
					&r, &g, &b);
			if (r >= l && g >= l && b >= l)
				cont = 0;
		}
		  xend--;
	}
  xend+=2;

  cont = 1;
	while (cont == 1)
	{
		if (yend <= 0)
			return image;
		for (size_t x = 0; x < width; x++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[yend * width + x], image->format,
					&r, &g, &b);
			if (r >= l && g >= l && b >= l)
				cont = 0;
		}
		  yend--;
	}
  yend+=2;

  xstart = xend - 1;
	cont = 1;
	while (cont == 1)
	{
		if (xstart <= 0)
			return image;
    cont = 0;
		for (size_t y = 0; y < height; y++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[y * width + xstart], image->format,
					&r, &g, &b);
			if (r >= l && g >= l && b >= l)
				cont = 1;
		}
      xstart--;
	}

  ystart = yend - 1;
  cont = 1;
	while (cont == 1)
	{
    if (ystart == 0)
      break;
    cont = 0;
		for (size_t x = 0; x < width; x++)
		{
			Uint8 r, g, b;
			SDL_GetRGB(imagePixels[ystart * width + x], image->format,
					&r, &g, &b);
			if (r >= l && g >= l && b >= l)
				cont = 1;
		}
		  ystart--;
	}

  if (xend - xstart <= 0 || yend - ystart <= 0)
    return image;

	SDL_Surface *cell = SDL_CreateRGBSurface(0, xend - xstart, yend - ystart,
      32,0,0,0,0);

  if (xend-xstart <= 10 || xend-xstart <= 10)
  {
    SDL_FreeSurface(image);
    return cell;
  }

	Uint32* cellPixels = cell->pixels;

	for (size_t x = xstart; x < xend; x++)
	{
		for (size_t y = ystart; y < yend; y++)
        	{
         		cellPixels[(y - ystart) * (xend - xstart) + x - xstart] =
              imagePixels[y * width + x];
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

	double w10 = width * 21 / 100;
	double h10 = height * 21 / 100;
	size_t cellw = width - 2 * w10;
	size_t cellh = height - 2 * h10;

	SDL_Surface *cell = SDL_CreateRGBSurface(0, cellw, cellh, 32,0,0,0,0);
	Uint32* cellPixels = cell->pixels;

	for (size_t x = 0; x < cellw; x++)
	{
		for (size_t y = 0; y < cellh; y++)
        	{
         		cellPixels[x * cellh + y] =
              imagePixels[(size_t)((x + w10) * height + y + h10)];
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
	      yadd += ((double)(height % 9) / 9);
	      if (yadd >= 1)
	      {
		      y++;
		      yadd--;
	      }
        for (size_t x = 0; x < width; x += xincrem)
        {
	          xadd += ((double)(width % 9) / 9);
	          if (xadd >= 1)
	          {
              x++;
		          xadd--;
	          }
            if (y + yincrem <= height && x + xincrem <= width)
            {
		            SDL_Surface *cell = SDL_CreateRGBSurface(0, xincrem, yincrem,
				            32,0,0,0,0);
	            	Uint32* cellPixels = cell->pixels;

	            	// copy pixels
		            for (size_t a = x; a < x + xincrem; a++)
    		        {
     			        for (size_t b = y; b < y + yincrem; b++)
        		      {
         		      	cellPixels[(b - y) * xincrem + a-x] =
					        	imagePixels[b * width + a];
        		      }
    		        }

            		SDL_Surface *cell2 = remove_border(cell);
            		SDL_Surface *cell3 = extract(cell2);
                SDL_Surface *cell4 = resizeImage(cell3, 28, 28);

		             //save cell
		          char *cellName = malloc(20 * sizeof(char));
		          cellName[0] = 0;
		          strcat(cellName, "Cells/");

		          char num[256];
		          snprintf(num, sizeof(num), "%zu", numCell);
		          strcat(cellName, num);

		          strcat(cellName, ".png");

	          	IMG_SavePNG(cell4, cellName);

		          free(cellName);
		          SDL_FreeSurface(cell4);
		          numCell++;
            }

        }
    }
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp=IMG_Load(path);
    if (temp  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* newsurf =
      SDL_ConvertSurfaceFormat(temp,SDL_PIXELFORMAT_RGB888,0);
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
