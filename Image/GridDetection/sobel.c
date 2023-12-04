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
