#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* blackwhite)
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

            // If the window is resized, updates and redraws the diagonals.
           case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, t);
                }
                break;
            case SDL_KEYDOWN:
            {
                if (t==colored)
                    t=blackwhite;
                else
                    t=colored;
                draw(renderer,t);
            }
        }
    }
}

// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
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

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
static Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = (r + g + b)/3;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    int temp =SDL_LockSurface(surface);
    if (temp!=0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    for (int i=0;i<len;i++ )
    {
        pixels[i]=pixel_to_grayscale(pixels[i],format);
    }
    SDL_UnlockSurface(surface);
}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Window* window = SDL_CreateWindow("Static Fractal Canopy", 0, 0,1,1,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Creates a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_SetWindowSize(window,surface->w,surface->h);
    SDL_Texture* colored = SDL_CreateTextureFromSurface(renderer,surface);
     if (colored  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    grayscale(surface);
    SDL_Texture* Black_white = SDL_CreateTextureFromSurface(renderer,surface);
    if (Black_white  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);

    event_loop(renderer,colored,Black_white);

    SDL_DestroyTexture(colored);
    SDL_DestroyTexture(Black_white);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // TODO:
    // - Initialize the SDL.
    // - Create a window.
    // - Create a renderer.
    // - Create a surface from the colored image.
    // - Resize the window according to the size of the image.
    // - Create a texture from the colored surface.
    // - Convert the surface into grayscale.
    // - Create a new texture from the grayscale surface.
    // - Free the surface.
    // - Dispatch the events.
    // - Destroy the objects.

    return EXIT_SUCCESS;
}
