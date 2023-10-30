#include <pixel.h>
#include <err.h>


static inline Uint8* pixelRef(SDL_Surface* s, unsigned int x, unsigned int y)
{
    int BytesPerPixel = s->format->BytesPerPixel;
    return (Uint8*)s->pixels + y * s->pitch + x * BytesPerPixel;
}

Uint32 get_pixel(SDL_Surface* s, unsigned int x, unsigned int y)
{
    Uint8* p = pixel_ref(s, x ,y);

    switch (surface->format->BytesPerPixel)
    {
        case 1:
            return *p;
        case 2:
            return *(Uint16*)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32*)p;
    }

    return 0;
}
