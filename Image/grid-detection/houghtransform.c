#include "houghtransform.h"

#define THRESHOLD 0.5

unsigned int** initMat(unsigned int x, unsigned int y)
{
    unsigned int** mat = NULL;
    mat = calloc(y + 1, sizeof(unsigned int*));

    if (mat == NULL)
        errx(1, "memory error");

    for(size_t i = 0; i < y; i++)
    {
        mat[i] = calloc(x + 1, sizeof(unsigned int*));
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


void draw_line(Image *image, int width, int height, Line *line, Pixel *color)
{
    int x0 = line->x0;
    int y0 = line->y0;

    int x1 = line->x1;
    int y1 = line->y1;

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;

    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int err = dx + dy;

    while(1)
    {
        if (0 <= x0 && x0 < w && 0 <= y0 && y0 < h)
        {
            image->pixels[x0][y0].r = color->r;
            image->pixels[x0][y0].g = color->g;
            image->pixels[x0][y0].b = color->b;
        }

        if (x0 == x1 && y0 == y1)
            break;

        int error2 = 2 * err;

        if (error2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (error2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}


void houghtransform(Image *image, Image *draw_image)
{
    /*
     * for now the return type is void. It will be changed
     *
     * for now there is no parameter. It will be changed later the function
     * should return a list of all the lines of the sudoku grid
     *
     * Parameters :
     * -----------------------------------------------------------------------
     * Image *image : a pointer on the image on which the hough transform
     *                will be applied
     * Image *draw_image : a pointer to the image on which the hough curve
     *                     will be drawn
     *
    */


    // image dimension
    const double width  = image->width;
    const double height = image->height;

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

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (image->pixels[x][y].r == 255)
            {
                for (int i = 0; i <= arrlen; i++)
                {
                    rho = x * cos(deg2rad(arr_theta[i])) +
                        y * sin(deg2rad(arr_theta[i]));
                    rho_index = rho + diagonal;
                    accumulator[rho_index][i]++;
                    if (accumulator[rho_index][i] > max)
                        max = = accumulator[rho_index][i];
                }
            }
        }
    }

    // line threshold computation
    int lineThreshold = max * THRESHOLD;

    // creating the lines and drawing them on the draw_image.
    int prev = 0;
    int prev_theta = 0;
    int prev_rho = 0;
    int increase = 1;

    // pixel color for the lines
    Pixel pixel = { .r = 200, .g = 0, .b = 200 };

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

                dot d0, d1, d2;

                d0.x = (int)(c * r);
                d0.y = (int)(s * r);

                d1.x = d0.x + (int)(diagonal * (-s));
                d1.y = d0.x + (int)(diagonal * c);

                d2.x = d0.x - (int)(diagonal * (-s));
                d2.y = d0.x - (int)(diagonal * c);

                Line line;
                line.x0 = d1.x;
                line.y0 = d1.y;
                line.x1 = d2.x;
                line.y1 = d2.y;

                draw_line(draw_image, width, height, &line, &pixel);

            }

        }
    }


}
