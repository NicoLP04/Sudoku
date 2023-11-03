#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <err.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;

// Function to load an image and create a surface
SDL_Surface* loadSurface(char* path)
{
    	SDL_Surface* loadedSurface = IMG_Load(path);
    	if (loadedSurface == NULL)
	{
printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    	}
    	return loadedSurface;
}


SDL_Texture* loadTexture(char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error:%s\n",
        path, IMG_GetError());
        return NULL;
    }

    SDL_Texture* newTexture =
SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (newTexture == NULL) {
        printf("Unable to create texture from %s ! SDL Error: %s\n",
        path, SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}




Uint32 getPixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    //printf("%" PRIu8 "\n",*p);
    switch (bpp)
    {
        case 1:
            //printf("1");
            return (Uint32)*p;
            //return *p;
        case 2:
            //printf("2");
            return *(Uint16*)p;
        case 3:
            //printf("3");
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                return p[0] << 16 | p[1] << 8 | p[2];
            } else {
                return p[0] | p[1] << 8 | p[2] << 16;
            }
        case 4:
            //printf("4");
            return *(Uint32*)p;
        default:
            //printf("default");
            return *p;
    }
}


// Function to put a pixel color on a surface
void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
        case 1:
            *p = (Uint8)pixel;
            break;
        case 2:
            *(Uint16*)p = (Uint16)pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = pixel & 0xFF;
            } else {
                p[0] = pixel & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = (pixel >> 16) & 0xFF;
            }
            break;
        case 4:
            *(Uint32*)p = pixel;
            break;
    }
}

Uint32 bilinearInterpolation(SDL_Surface* surface, double x, double y) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    double alpha = x - x0;
    double beta = y - y0;

    Uint32 pixel00 = getPixel(surface, x0, y0);
    Uint32 pixel10 = getPixel(surface, x1, y0);
    Uint32 pixel01 = getPixel(surface, x0, y1);
    Uint32 pixel11 = getPixel(surface, x1, y1);

    Uint8 r = (1 - alpha) * (1 - beta) * ((pixel00 >> 16) & 0xFF) +
               alpha * (1 - beta) * ((pixel10 >> 16) & 0xFF) +
               (1 - alpha) * beta * ((pixel01 >> 16) & 0xFF) +
               alpha * beta * ((pixel11 >> 16) & 0xFF);

    Uint8 g = (1 - alpha) * (1 - beta) * ((pixel00 >> 8) & 0xFF) +
               alpha * (1 - beta) * ((pixel10 >> 8) & 0xFF) +
               (1 - alpha) * beta * ((pixel01 >> 8) & 0xFF) +
               alpha * beta * ((pixel11 >> 8) & 0xFF);

    Uint8 b = (1 - alpha) * (1 - beta) * (pixel00 & 0xFF) +
               alpha * (1 - beta) * (pixel10 & 0xFF) +
               (1 - alpha) * beta * (pixel01 & 0xFF) +
               alpha * beta * (pixel11 & 0xFF);

    return SDL_MapRGB(surface->format, r, g, b);
}


// Function to rotate a surface by a given angle (in degrees)
SDL_Surface* rotateSurface(SDL_Surface* srcSurface, double angle)
{
    if (srcSurface == NULL)
    {
        return NULL;
    }
    angle = 360- angle;
    double radAngle = angle * M_PI / 180.0;
    int newWidth = (int)(fabs(srcSurface->w * cos(radAngle)) +
 fabs(srcSurface->h * sin(radAngle)));
    int newHeight = (int)(fabs(srcSurface->w * sin(radAngle)) +
 fabs(srcSurface->h * cos(radAngle)));


    // Create a copy of the source surface
    SDL_Surface* rotatedSurface = SDL_CreateRGBSurfaceWithFormat(0,newWidth,
 newHeight, srcSurface->format->BitsPerPixel, srcSurface->format->format);
    if (rotatedSurface == NULL) {
        printf("Unable to create rotated surface! SDL Error: %s\n",
SDL_GetError());
        return NULL;
    }
    int minX = newWidth;
    int minY = newHeight;
    int maxX = -1;
    int maxY = -1;
	int centerXSrc = srcSurface->w / 2;
    int centerYSrc = srcSurface->h / 2;
    int centerXDst = newWidth / 2;
    int centerYDst = newHeight / 2;

    // Loop through the pixels of the rotated surface and copy from the source
    for (int x = 0; x < newWidth; x++)
    {
        for (int y = 0; y < newHeight; y++)
        {
            int srcX = (int)((x - centerXDst) * cos(radAngle) -
 (y - centerYDst) * sin(radAngle) + centerXSrc);
            int srcY = (int)((x - centerXDst) * sin(radAngle) +
 (y - centerYDst) * cos(radAngle) + centerYSrc);
            // Check if the source coordinates are within bounds
            if (srcX >= 0 && srcX < srcSurface->w &&
srcY >= 0 && srcY < srcSurface->h)
            {
                //Uint32 pixel = getPixel(srcSurface, srcX, srcY);
                Uint32 pixel = bilinearInterpolation(srcSurface, srcX, srcY);
		        if (pixel != 0xFFFFFFFF)
		        {
			        putPixel(rotatedSurface, x, y, pixel);
		            if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
            else
            {
                putPixel(rotatedSurface, x, y, 0xFFFFFFFF);
            }
        }
    }
    int boxWidth = maxX - minX + 1;
    int boxHeight = maxY - minY + 1;
    SDL_Surface* finalRotatedSurface = SDL_CreateRGBSurfaceWithFormat(0,
boxWidth, boxHeight, srcSurface->format->BitsPerPixel,
srcSurface->format->format);
    if (finalRotatedSurface == NULL) {
        printf("Unable to create rotated surface! SDL Error: %s\n",
SDL_GetError());
        return NULL;
    }
    // Copy the minimum bounding box from the rotated
//surface to the final surface
    SDL_Rect boxRect = {minX, minY, boxWidth, boxHeight};
    SDL_BlitSurface(rotatedSurface, &boxRect, finalRotatedSurface, NULL);
    // Free the temporary rotated surface
    SDL_FreeSurface(rotatedSurface);
    return finalRotatedSurface;

}

SDL_Surface* convert1bppTo3bpp(SDL_Surface* srcSurface) {

    SDL_Surface* newSurface = SDL_CreateRGBSurfaceWithFormat(0,
srcSurface->w, srcSurface->h, 24, SDL_PIXELFORMAT_RGB24);
    if (newSurface == NULL) {
        return NULL;
    }

    for (int x = 0; x < srcSurface->w; x++)
    {
        for (int y = 0; y < srcSurface->h; y++)
        {
            Uint8* pixels = (Uint8*)srcSurface->pixels;
            Uint8 pixelIndex = pixels[y * srcSurface->pitch + x];
            SDL_Color* paletteColor =
&srcSurface->format->palette->colors[pixelIndex];
            Uint32 newColor = SDL_MapRGB(newSurface->format,
paletteColor->r, paletteColor->g, paletteColor->b);
            putPixel(newSurface, x, y, newColor);
        }
    }

    return newSurface;
}

int main(int argc, char* args[]) {
	if (argc != 3)
		errx(EXIT_FAILURE, "Usage: image-file / angle");
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Initialize SDL_image for image loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	errx(EXIT_FAILURE, "%s", SDL_GetError());


    // Create a window
    gWindow = SDL_CreateWindow("SDL Rotate",0,0,0,0 ,
SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);

    if (gWindow == NULL)
	errx(EXIT_FAILURE, "%s", SDL_GetError());

    int w,h;
    w = 540;
    h = 540;
    SDL_SetWindowSize(gWindow, w,h);

    // Create a renderer (not used in this example)
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL)
	errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Load the image you want to rotate
    SDL_Surface* imageSurface = loadSurface(args[1]);
    if (imageSurface == NULL) {
        return 1;
    }
    if (imageSurface->format->BytesPerPixel == 1)
    {
        imageSurface = convert1bppTo3bpp(imageSurface);
    }

    // Angle of rotation (in degrees)
    double angle = strtol(args[2], NULL, 10);;

    SDL_Surface* rotatedSurface = rotateSurface(imageSurface, angle);

    gTexture = loadTexture(args[1], gRenderer);

    int quit = 1;
    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
	    {
            if (e.type == SDL_QUIT)	        {
                quit = 1;
            }
 	    }
	    // Clear the screen
	    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	    SDL_RenderClear(gRenderer);
	    // Render the rotated texture
	    SDL_RenderCopyEx(gRenderer,
gTexture, NULL, NULL, angle, NULL, SDL_FLIP_NONE);
	    // Update the screen
	    SDL_RenderPresent(gRenderer);
        // Update the screen
        //SDL_UpdateWindowSurface(gWindow);
    }

    //IMG_SavePNG(rotatedSurface, "rotated.png");
    IMG_SavePNG(rotatedSurface, "rotated.png");
    // Clean up and quit
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(rotatedSurface);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
