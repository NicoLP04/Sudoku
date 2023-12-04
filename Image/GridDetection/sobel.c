#include "sobel.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to apply the Sobel filter to the image
void applySobel(SDL_Surface *input, SDL_Surface *output)
{
    int width = input->w;
    int height = input->h;

    Uint32 *pixels = (Uint32 *)input->pixels;
    Uint32 *outputPixels = (Uint32 *)output->pixels;

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int gx = 0, gy = 0;

            // Apply Sobel filter
            gx += (pixels[(y - 1) * width + (x - 1)] & 0xFF) * -1;
            gx += (pixels[(y - 1) * width + x] & 0xFF) * -2;
            gx += (pixels[(y - 1) * width + (x + 1)] & 0xFF) * -1;
            gx += (pixels[y * width + (x - 1)] & 0xFF) * 0;
            gx += (pixels[y * width + x] & 0xFF) * 0;
            gx += (pixels[y * width + (x + 1)] & 0xFF) * 0;
            gx += (pixels[(y + 1) * width + (x - 1)] & 0xFF) * 1;
            gx += (pixels[(y + 1) * width + x] & 0xFF) * 2;
            gx += (pixels[(y + 1) * width + (x + 1)] & 0xFF) * 1;

            gy += (pixels[(y - 1) * width + (x - 1)] & 0xFF) * -1;
            gy += (pixels[(y - 1) * width + x] & 0xFF) * 0;
            gy += (pixels[(y - 1) * width + (x + 1)] & 0xFF) * 1;
            gy += (pixels[y * width + (x - 1)] & 0xFF) * -2;
            gy += (pixels[y * width + x] & 0xFF) * 0;
            gy += (pixels[y * width + (x + 1)] & 0xFF) * 2;
            gy += (pixels[(y + 1) * width + (x - 1)] & 0xFF) * -1;
            gy += (pixels[(y + 1) * width + x] & 0xFF) * 0;
            gy += (pixels[(y + 1) * width + (x + 1)] & 0xFF) * 1;

            int magnitude = (int)sqrt(gx * gx + gy * gy);

            // Clamp the values to 255 to prevent overflow
            magnitude = (magnitude > 255) ? 255 : magnitude;

            // Set the pixel value in the output image
            outputPixels[y * width + x] =
                SDL_MapRGB(output->format, magnitude, magnitude, magnitude);
        }
    }
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp = IMG_Load(path);
    if (temp == NULL)
    {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface* ret = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
    if (ret == NULL)
    {
        fprintf(stderr, "Error converting surface format: %s\n", SDL_GetError());
        SDL_FreeSurface(temp);
        return NULL;
    }

    SDL_FreeSurface(temp);

    return ret;
}



void save_texture(const char* file_name, SDL_Renderer* renderer,
        SDL_Texture* texture)
{
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
            0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format,
            surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("%s\n",argv[1]);
    char *filename = argv[1];

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL initialization error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    printf("Before load_image\n");
    SDL_Surface *image = load_image(filename);
    printf("After load_image\n");

    if (!image)
    {
        fprintf(stderr, "Error loading image\n");
        return EXIT_FAILURE;
    }

    SDL_Surface *output =
        SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);

    applySobel(image, output);

    SDL_Window *window = SDL_CreateWindow("Sobel Edge Detection",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image->w, image->h,
        SDL_WINDOW_SHOWN);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }


    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(renderer, output);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Save the output image using the save_texture function
    save_texture("output.png", renderer, texture);

    SDL_Delay(3000);  // Wait for 3 seconds

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_FreeSurface(image);
    SDL_FreeSurface(output);

    SDL_Quit();

    return 0;
}
