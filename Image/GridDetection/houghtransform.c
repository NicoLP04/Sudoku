#include "houghtransform.h"
#include "pixel.h"

#define THRESHOLD 0.5


void* line2voidptr(Line line)
{
    void* ptr = malloc(sizeof(Line));

    if (ptr == NULL)
        errx(EXIT_FAILURE, "line2voidptr: malloc failed!");

    *(Line *)ptr = line;

    return ptr;
}


unsigned int** initMat(unsigned int x, unsigned int y)
{
    unsigned int** mat = NULL;
    mat = calloc(y + 1, sizeof(unsigned int*));

    if (mat == NULL)
        errx(1, "memory error");

    for (size_t i = 0; i < y; i++)
    {
        mat[i] = calloc(x + 1, sizeof(unsigned int));
        if (mat[i] == NULL)
            errx(1, "memory error");
    }

    return mat;
}

void freeMat(unsigned int** mat, double len)
{
    for(size_t i = 0; i < len; ++i)
        free(mat[i]);
    free(mat);
}


double deg2rad(double degrees)
{
    return degrees * M_PI / 180.0;
}


double rad2deg(double radian)
{
    return radian * 180.0 / M_PI;
}


void array_fill(double* arr, double len, double step, double maxVal,
        double minVal)
{
    int i = 0;
    for (double val = minVal; val <= maxVal && i < len; val += step, ++i)
        arr[i] = val;
}


List houghtransform(SDL_Surface* image, SDL_Renderer* draw_image)
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

    // compute the length of the arrays
    const double arrlen = 2 * diagonal;

    // create the arrays
    double rho_step = (maxRho - minRho) / arrlen;
    double* arr_rhos = calloc(arrlen + 1, sizeof(double));

    array_fill(arr_rhos, arrlen, rho_step, maxRho, minRho);

    double theta_step = (maxTheta - minTheta) / arrlen;
    double* arr_theta = calloc(arrlen + 1, sizeof(double));

    array_fill(arr_theta, arrlen, theta_step, maxTheta, minTheta);

    // initialize the accumulator matrix
    unsigned int** accumulator = initMat(arrlen + 1, arrlen + 1);

    // fill the accumulator matrix with all the values.
    // during the filling we find the max value.
    double rho;
    int rho_index;
    unsigned int max = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Uint32 p = get_pixel(image, x, y);
            SDL_Color rgb;
            SDL_GetRGB(p, image->format, &rgb.r, &rgb.g, &rgb.b);
            if (rgb.r >= 175 && rgb.g >= 175 && rgb.b >= 175)
            {
                for (int i = 0; i <= arrlen; i++)
                {
                    rho = x * cos((arr_theta[i])) +
                        y * sin((arr_theta[i]));
                    rho_index = rho + diagonal;
                    accumulator[rho_index][i]++;
                    if (accumulator[rho_index][i] > max)
                        max = accumulator[rho_index][i];
                }
            }
        }
    }

    // line threshold computation
    int lineThreshold = max * THRESHOLD;

    // creating the lines and drawing them on the draw_image.
    int prev = accumulator[0][0];
    int prev_theta = 0;
    int prev_rho = 0;
    int increase = 1;

    List lines = { NULL, NULL, 0 };


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

                double c = cos(t);
                double s = sin(t);

                int x = (int)(c * r);
                int y = (int)(s * r);

                int x1 = x + (int)(diagonal * (-s));
                int y1 = y + (int)(diagonal * c);

                int x2 = x - (int)(diagonal * (-s));
                int y2 = y - (int)(diagonal * c);

                Line line;
                line.X0 = x1;
                line.Y0 = y1;
                line.X1 = x2;
                line.Y1 = y2;
                line.theta = t;

                void* p = line2voidptr(line);

                appendValue(&lines, p);

                int draw = 1;
                if (draw)
                {
                    // set draw color to magenta
                    SDL_SetRenderDrawColor(draw_image, 200, 0, 200, 255);
                    // draw the line
                    SDL_RenderDrawLine(draw_image, x1, y1, x2, y2);
                }

            }
        }
    }


    free(arr_rhos);
    free(arr_theta);
    freeMat(accumulator, arrlen);

    return lines;
}
