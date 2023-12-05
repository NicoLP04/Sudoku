#include "image.h"


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp = IMG_Load(path);
    if (temp == NULL)
    {
        fprintf(stderr, "Error loading image: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface* ret =
        SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
    if (ret == NULL)
    {
        fprintf(stderr, "Error converting surface format: %s\n",
            SDL_GetError());
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
