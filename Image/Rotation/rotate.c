#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <err.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

SDL_Window* gWindow = NULL;

// Function to load an image and create a surface
SDL_Surface* loadSurface(char* path)
{
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n",
        path, IMG_GetError());
    }
    return loadedSurface;
}


SDL_Texture* loadTexture(char* path, SDL_Renderer* renderer)
{
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error:%s\n",
        path, IMG_GetError());
        return NULL;
    }

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer,
    loadedSurface);
    if (newTexture == NULL)
    {
        SDL_FreeSurface(loadedSurface);
        printf("Unable to create texture from %s ! SDL Error: %s\n",
        path, SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}




Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
    int bytes = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)(surface->pixels + y * surface->pitch + x * bytes);
    switch (bytes)
    {
        case 1:
            return *p;
        case 2:
            return *(Uint16*)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                return p[0] << 16 | p[1] << 8 | p[2];
            }
            else
            {
                return p[0] | p[1] << 8 | p[2] << 16;
            }
        case 4:
            return *(Uint32*)p;
        default:
            return *p;
    }
}


// Function to put a pixel color on a surface
void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    int bytes = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bytes;
    switch(bytes)
    {
        case 1:
            *p = (Uint8)pixel;
            break;
        case 2:
            *(Uint16*)p = (Uint16)pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = pixel & 0xFF;
            }
            else
            {
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

// Function to rotate a surface by a given angle (in degrees)
SDL_Surface* rotateSurface(SDL_Surface* surface, double angle)
{
    if (surface == NULL)
        return NULL;
    angle = 360- angle;
    double radAngle = angle * M_PI / 180.0;

    int newWidth = (int)(fabs(surface->w * cos(radAngle)) +
    fabs(surface->h * sin(radAngle)));
    int newHeight = (int)(fabs(surface->w * sin(radAngle)) +
    fabs(surface->h * cos(radAngle)));

    // Create a copy of the source surface
    SDL_Surface* rotatedSurface = SDL_CreateRGBSurfaceWithFormat(0,newWidth,
    newHeight, surface->format->BitsPerPixel, surface->format->format);

    if (rotatedSurface == NULL)
    {
        printf("Unable to create rotated surface! SDL Error: %s\n",
        SDL_GetError());
        return NULL;
    }

    int centerXSrc = surface->w / 2;
    int centerYSrc = surface->h / 2;
    int maxX = -1;
    int maxY = -1;
    int minX = newWidth;
    int minY = newHeight;
    int centerXDst = newWidth / 2;
    int centerYDst = newHeight / 2;

    SDL_LockSurface(rotatedSurface);
    for (int x = 0; x < newWidth; x++)
    {
        for (int y = 0; y < newHeight; y++)
        {
            int srcX = (int)((x - centerXDst) * cos(radAngle) -
            (y - centerYDst) * sin(radAngle) + centerXSrc);
            int srcY = (int)((x - centerXDst) * sin(radAngle) +
            (y - centerYDst) * cos(radAngle) + centerYSrc);
            if (srcX >= 0 && srcX < surface->w &&
            srcY >= 0 && srcY < surface->h) //Is placeable ?
            {
                Uint32 pixel = getPixel(surface, srcX, srcY);
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
    SDL_UnlockSurface(rotatedSurface);
    int boxWidth = maxX - minX + 1;
    int boxHeight = maxY - minY + 1;
    SDL_Surface* finalRotatedSurface = SDL_CreateRGBSurfaceWithFormat(0,
    boxWidth, boxHeight, surface->format->BitsPerPixel,
surface->format->format);

    if (finalRotatedSurface == NULL) {
        printf("Unable to create rotated surface! SDL Error: %s\n",
        SDL_GetError());
        return NULL;
    }

    SDL_Rect boxRect = {minX, minY, boxWidth, boxHeight};
    SDL_BlitSurface(rotatedSurface, &boxRect, finalRotatedSurface, NULL);
    SDL_FreeSurface(rotatedSurface);
    return finalRotatedSurface;

}

SDL_Surface* conv1bppTo3bpp(SDL_Surface* surface)
{
    SDL_Surface* newSurface = SDL_CreateRGBSurfaceWithFormat(0,
    surface->w, surface->h, 24, SDL_PIXELFORMAT_RGB24);
    if (newSurface == NULL)
    {
        return NULL;
    }
    for (int x = 0; x < surface->w; x++)
    {
        for (int y = 0; y < surface->h; y++)
        {
            Uint8* pixels = (Uint8*)surface->pixels;
            Uint8 pixelIndex = pixels[y * surface->pitch + x];
            SDL_Color* color = &surface->format->palette->colors[pixelIndex];
            Uint32 newColor =
            SDL_MapRGB(newSurface->format,color->r, color->g, color->b);
            putPixel(newSurface, x, y, newColor);
        }
    }
    SDL_FreeSurface(surface);
    return newSurface;
}

int main(int argc, char* args[])
{
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: %s image-file / angle", args[0]);

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

    int w = 540;
    int h = 540;
    SDL_SetWindowSize(gWindow, w,h);
    double angle = strtol(args[2], NULL, 10);;

    SDL_Surface* imageSurface = loadSurface(args[1]);
    if (imageSurface == NULL)
    {
        errx(EXIT_FAILURE,
        "Unable to load image from '%s'! SDL_image Error: %s\n",
        args[1], IMG_GetError());
    }
    if (imageSurface->format->BytesPerPixel == 1)
    {
        SDL_Surface* newSurf = conv1bppTo3bpp(imageSurface);
        SDL_Surface* rotatedSurface = rotateSurface(newSurf, angle);
        IMG_SavePNG(rotatedSurface, "rotated.png");
        SDL_FreeSurface(newSurf);
        SDL_FreeSurface(rotatedSurface);
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return 0;
    }
    SDL_Surface* rotatedSurface = rotateSurface(imageSurface, angle);
    IMG_SavePNG(rotatedSurface, "rotated.png");
    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(rotatedSurface);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();
    return 0;
}