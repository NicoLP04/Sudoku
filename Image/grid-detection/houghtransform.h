#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Dot
{
    int x;
    int y;
} Dot;

typedef struct Line
{
    int x0;
    int y0;
    int x1;
    int y1;
    double theta;
} Line;
