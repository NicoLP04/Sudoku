#include "houghtransform.h"
#include "pixel.h"

#define THRESHOLD 0.5

unsigned int** initMat(unsigned int x, unsigned int y)
{
    unsigned int** mat = NULL;
    mat = calloc(y + 1, sizeof(unsigned int*));

    if (mat == NULL)
        errx(1, "memory error");

    for(size_t i = 0; i < y; i++)
    {
        mat[i] = calloc(x + 1, sizeof(unsigned int));
        if (mat[i] == NULL)
            errx(1, "memory error");
    }

    return mat;
}


double deg2rad(double degrees)
{
    return degrees * M_PI / 180.0;
}


double rad2deg(double radian)
{
    return radian * 180.0 / M_PI;
}



void houghtransform(SDL_Surface* image, SDL_Renderer* draw_image)
{

    // image dimension
    const double width  = image->w;
    const double height = image->h;

    // compute the diagonal
    const double diagonal = sqrt(width * width + height * height);

    // initialize rho and theta
    const double maxTheta = 180.0;
    const double minTheta = 0.0;

    const double maxRho = diagonal;
    const double minRho = -diagonal;

    const double arrlen = 2 * diagonal;

    // create the arrays

    double rho_step = (maxRho - minRho) / arrlen;

    double* arr_rhos = calloc(arrlen + 1, sizeof(double));

    int index = 0;
    for (double val = minRho; val <= maxRho && index < arrlen;
            val += rho_step, index++)
    {
        arr_rhos[index] = val;
    }

    double theta_step = (maxTheta - minTheta) / arrlen;

    double* arr_theta = calloc(arrlen + 1, sizeof(double));

    index = 0;
    for (double val = minTheta; val <= maxTheta && index < arrlen;
            val += theta_step, index++)
    {
        arr_theta[index] = val;
    }

    // initialize the accumulator matrix
    unsigned int** accumulator = initMat(arrlen + 1, arrlen + 1);

    // fill the accumulator matrix with all the values.
    // during the filling we find the max value.
    double rho;
    int rho_index;
    unsigned int max = 0;

    int err = SDL_LockSurface(image);
    if (err != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 p = get_pixel(image, x, y);
            SDL_Color rgb;
            SDL_GetRGB(p, image->format, &rgb.r, &rgb.g, &rgb.b);
            if (rgb.r == 255)
            {
                for (int i = 0; i <= arrlen; i++)
                {
                    rho = x * cos(deg2rad(arr_theta[i])) +
                        y * sin(deg2rad(arr_theta[i]));
                    rho_index = rho + diagonal;
                    accumulator[rho_index][i]++;
                    if (accumulator[rho_index][i] > max)
                        max = accumulator[rho_index][i];
                }
            }
        }
    }

    SDL_UnlockSurface(image);

    // line threshold computation
    int lineThreshold = max * THRESHOLD;

    // creating the lines and drawing them on the draw_image.
    int prev = accumulator[0][0];
    int prev_theta = 0;
    int prev_rho = 0;
    int increase = 1;


    for (int theta = 0; theta <= arrlen; theta++)
    {
        for (int rho = 0; rho <= arrlen; rho++)
        {
            int val = accumulator[rho][theta];

            if (val >= prev)
            {
                prev = val;
                prev_rho = rho;
                prev_theta = theta;
                increase = 1;
                continue;
            }
            else if (val < prev && increase)
            {
                increase = 0;
            }
            else if (val < prev)
            {
                prev = val;
                prev_rho = rho;
                prev_theta = theta;
                increase = 0;
                continue;
            }

            if (val >= lineThreshold)
            {
                double r = arr_rhos[prev_rho];
                double t = arr_theta[prev_theta];

                // drawing the line on the draw image.

                double c = cos(t);
                double s = sin(t);


                int x = (int)(c * r);
                int y = (int)(s * r);

                int x1 = x + (int)(diagonal * (-s));
                int y1 = y + (int)(diagonal * c);

                int x2 = x - (int)(diagonal * (-s));
                int y2 = y - (int)(diagonal * c);

                printf("line : x1 = %4i, y1 = %4i / x2 = %4i, y2 = %4i\n", x1, y1, x2, y2);

                // set draw color to magenta
                SDL_SetRenderDrawColor(draw_image, 200, 0, 200, 255);
                // draw the line
                SDL_RenderDrawLine(draw_image, x1, y1, x2, y2);
            }
        }
    }

    free(arr_rhos);
    free(arr_theta);

    for (size_t i = 0; i < arrlen; ++i)
        free(accumulator[i]);
    free(accumulator);
}



SDL_Surface* load_image(const char* path)
{
	SDL_Surface* temp = IMG_Load(path);
	if (temp == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_Surface* ret = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGB888, 0);
	if (ret == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	SDL_FreeSurface(temp);

	return ret;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Error: expected 1 argument, got: %i\n", argc - 1);
        printf("Usage: ./COMMAND <PATH>\n");
        return EXIT_FAILURE;
    }

    // initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // create widow
    SDL_Window* window = SDL_CreateWindow("image display", 200, 200, 700, 700,
            SDL_WINDOW_SHOWN);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // load the image
    SDL_Texture* imageTexture = IMG_LoadTexture(renderer, argv[1]);
    if (imageTexture == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // Create a surface to detect the grid
    SDL_Surface* image = load_image(argv[1]);
    if (image == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    // create a texture to draw on
    SDL_Texture* targetTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w, image->h);

    // set the target texture
    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    // Draw the original image onto the resultTexture
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    // Apply grid detection algorithm
    houghtransform(image, renderer);

    // reset the target to the default renderer
    SDL_SetRenderTarget(renderer, NULL);

    // clear the screen
    SDL_RenderClear(renderer);

    // copy the image texture to the renderer
    SDL_RenderCopy(renderer, targetTexture, NULL, NULL);

    // Present the result
    SDL_RenderPresent(renderer);

    int quit = 0;
    SDL_Event e;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
            if (e.type == SDL_QUIT)
                quit = 1;
    }

    // Quit SDL
    SDL_FreeSurface(image);
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(targetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
