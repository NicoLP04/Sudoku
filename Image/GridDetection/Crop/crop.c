#include "crop.h"

typedef struct {
    double matrix[3][3];
} HomographyMatrix;

// Helper function to get pixel color at (x, y) from a surface
Uint32 get_pixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
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
        default:
            return 0; // Shouldn't happen, but just in case
    }
}

// Helper function to set pixel color at (x, y) in a surface
void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16*)p = pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = pixel & 0xFF;
            } else {
                p[0] = pixel & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = (pixel >> 16) & 0xFF;
            }
            break;
        case 4:
            *(Uint32*)p = pixel;
            break;
        default:
            // Shouldn't happen, but just in case
            break;
    }
}

SDL_Surface* transposeImage(SDL_Surface* inputImage, double matrix[3][3], double length) {
    // Create a new surface for the transposed image
    SDL_Surface* outputImage = SDL_CreateRGBSurface(0, length, length,
                                                   inputImage->format->BitsPerPixel,
                                                   inputImage->format->Rmask,
                                                   inputImage->format->Gmask,
                                                   inputImage->format->Bmask,
                                                   inputImage->format->Amask);

    if (outputImage == NULL) {
        fprintf(stderr, "Unable to create surface: %s\n", SDL_GetError());
        return NULL;
    }

    // Apply the transpose operation
    for (int yo = 0; yo < outputImage->h; ++yo) {
        for (int xo = 0; xo < outputImage->w; ++xo) {
            double idk = xo * matrix[2][0] + yo * matrix[2][1] + matrix[2][2];
            int x = (xo * matrix[0][0] + yo * matrix[0][1] + matrix[0][2]) / idk;
            int y = (xo * matrix[1][0] + yo * matrix[1][1] + matrix[1][2]) / idk;

            if (x >= 0 && y >= 0 && x < inputImage->w && y < inputImage->h)
            {
                Uint32 pixel = get_pixel(inputImage, x, y);
                set_pixel(outputImage, xo, yo, pixel);
            }
        }
    }

    return outputImage;
}


double determinant(double A[3][3]) {
    return A[0][0] * (A[1][1] * A[2][2] - A[2][1] * A[1][2]) -
           A[0][1] * (A[1][0] * A[2][2] - A[2][0] * A[1][2]) +
           A[0][2] * (A[1][0] * A[2][1] - A[2][0] * A[1][1]);
}


void invertMatrix_3x3(double A[3][3], double result[3][3]) {
    double det = determinant(A);

    if (det == 0) {
        printf("Matrix is singular. Cannot invert.\n");
        return;
    }

    // Adjugate matrix
    result[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) / det;
    result[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) / det;
    result[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / det;
    result[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) / det;
    result[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / det;
    result[1][2] = (A[1][0] * A[0][2] - A[0][0] * A[1][2]) / det;
    result[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) / det;
    result[2][1] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) / det;
    result[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) / det;
}


void Minor(double minorMatrix[9][9], int colMatrix, int sizeMatrix,
           double newMinorMatrix[9][9])
{
    int col, row, row2 = 0, col2 = 0;
    for (row = 1; row < sizeMatrix; row++)
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            if (col == colMatrix)
            {
                continue;
            }
            newMinorMatrix[row2][col2] = minorMatrix[row][col];
            col2++;
            if (col2 == (sizeMatrix - 1))
            {
                row2++;
                col2 = 0;
            }
        }
    }
    return;
}


double Determinte(double minorMatrix[9][9], int sizeMatrix)
{
    int col;
    double sum = 0, newMinorMatrix[9][9];
    if (sizeMatrix == 1)
    {
        return minorMatrix[0][0];
    }
    else if (sizeMatrix == 2)
    {
        return (minorMatrix[0][0] * minorMatrix[1][1]
                - minorMatrix[0][1] * minorMatrix[1][0]);
    }
    else
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            Minor(minorMatrix, col, sizeMatrix, newMinorMatrix);
            sum += (double)(minorMatrix[0][col] * pow(-1, col)
                            * Determinte(newMinorMatrix,
                                         (sizeMatrix - 1)));
        }
    }
    return sum;
}

void Transpose(double cofactorMatrix[9][9], double sizeMatrix,
               double determinte, double coutMatrix[9][9],
               double transposeMatrix[9][9])
{
    int row, col;
    for (row = 0; row < sizeMatrix; row++)
    {
        for (col = 0; col < sizeMatrix; col++)
        {
            transposeMatrix[row][col] = cofactorMatrix[col][row];
            coutMatrix[row][col] =
                cofactorMatrix[col][row] / determinte;
        }
    }
    return;
}

void Cofactor(double cinMatrix[9][9], double sizeMatrix, double determinte,
              double coutMatrix[9][9], double transposeMatrix[9][9])
{
    double minorMatrix[9][9], cofactorMatrix[9][9];
    int col3, row3, row2, col2, row, col;
    for (row3 = 0; row3 < sizeMatrix; row3++)
    {
        for (col3 = 0; col3 < sizeMatrix; col3++)
        {
            row2 = 0;
            col2 = 0;
            for (row = 0; row < sizeMatrix; row++)
            {
                for (col = 0; col < sizeMatrix; col++)
                {
                    if (row != row3 && col != col3)
                    {
                        minorMatrix[row2][col2] = cinMatrix[row][col];
                        if (col2 < (sizeMatrix - 2))
                        {
                            col2++;
                        }
                        else
                        {
                            col2 = 0;
                            row2++;
                        }
                    }
                }
            }
            cofactorMatrix[row3][col3] = pow(-1, (row3 + col3))
                * Determinte(minorMatrix, (sizeMatrix - 1));
        }
    }
    Transpose(cofactorMatrix, sizeMatrix, determinte, coutMatrix,
              transposeMatrix);
    return;
}

void Inverse(double cinMatrix[9][9], int sizeMatrix, double determinte,
             double coutMatrix[9][9], double transposeMatrix[9][9])
{
    if (determinte == 0)
    {
        printf("\n Inverse of entered matrix is not possible \n");
    }
    else if (sizeMatrix == 1)
    {
        coutMatrix[0][0] = 1;
    }
    else
    {
        Cofactor(cinMatrix, sizeMatrix, determinte, coutMatrix,
                 transposeMatrix);
    }
    return;
}

void inverseMat(double cinMatrix[9][9], double coutMatrix[9][9], int sizeMatrix)
{
    double determinte, transposeMatrix[9][9];

    determinte = (double)Determinte(cinMatrix, sizeMatrix);

    Inverse(cinMatrix, sizeMatrix, determinte, coutMatrix, transposeMatrix);
}


// Function to compute the homography matrix
HomographyMatrix computeHomography(double src[][2], double dst[][2]) {
    HomographyMatrix H;

    // A is the coefficient matrix, B is the constant matrix
    double A[9][9] = {
        { -src[0][0], -src[0][1], -1, 0, 0, 0, src[0][0] * dst[0][0],
          src[0][1] * dst[0][0], dst[0][0] },
        { 0, 0, 0, -src[0][0], -src[0][1], -1, src[0][0] * dst[0][1],
          src[0][1] * dst[0][1], dst[0][1] },
        { -src[1][0], -src[1][1], -1, 0, 0, 0, src[1][0] * dst[1][0],
          src[1][1] * dst[1][0], dst[1][0] },
        { 0, 0, 0, -src[1][0], -src[1][1], -1, src[1][0] * dst[1][1],
          src[1][1] * dst[1][1], dst[1][1] },
        { -src[2][0], -src[2][1], -1, 0, 0, 0, src[2][0] * dst[2][0],
          src[2][1] * dst[2][0], dst[2][0] },
        { 0, 0, 0, -src[2][0], -src[2][1], -1, src[2][0] * dst[2][1],
          src[2][1] * dst[2][1], dst[2][1] },
        { -src[3][0], -src[3][1], -1, 0, 0, 0, src[3][0] * dst[3][0],
          src[3][1] * dst[3][0], dst[3][0] },
        { 0, 0, 0, -src[3][0], -src[3][1], -1, src[3][0] * dst[3][1],
          src[3][1] * dst[3][1], dst[3][1] },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1 }
    };
    double B[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    // Calculate the matrix inverse of A
    double invA[9][9] = { 0 };
    inverseMat(A, invA, 9);

    // Multiply the inverse of A with B to get the homography matrix
    double V[9] = { 0 };
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            V[i] += invA[i][j] * B[j];
        }
    }
    int k = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++, k++) {
            H.matrix[i][j] += V[k];
        }
    }

    return H;
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp=IMG_Load(path);
    SDL_Surface* newsurf =
      SDL_ConvertSurfaceFormat(temp,SDL_PIXELFORMAT_RGB888,0);
    SDL_FreeSurface(temp);
    return newsurf;
}


SDL_Surface *crop(SDL_Surface *image, double x1, double y1, double x2, double y2,
    double x3, double y3, double x4, double y4)
{
    double src[][2] = {{x1, y1}, {x2, y2}, {x3, y3}, {x4, y4}};

    double l1 = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    double l2 = sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
    double l3 = sqrt((x4 - x3) * (x4 - x3) + (y4 - y3) * (y4 - y3));
    double l4 = sqrt((x1 - x4) * (x1 - x4) + (y1 - y4) * (y1 - y4));
    double length = fmax(fmax(l1, l2), fmax(l3, l4));
    double dst[][2] = {{0, 0}, {length, 0}, {length, length}, {0, length}};

    // Compute the homography matrix
    HomographyMatrix res = computeHomography(src, dst);
    double H[3][3] = { 0 };

    invertMatrix_3x3(res.matrix, H);

	  SDL_Surface *newImage = transposeImage(image, H, length);
	  SDL_FreeSurface(image);

    return newImage;
}

/*
int main(int argc, char** argv) {
    SDL_Surface *image = load_image(argv[1]);
    printf("%d, %d\n", image->w, image->h);

    //SDL_Surface* newImage = crop(image, 335, 214, 1149, 207, 1159, 1028, 337, 1030); // 2
    SDL_Surface* newImage = crop(image, 128, 88, 649, 87, 650, 607, 130, 608); // 3
    //SDL_Surface* newImage = crop(image, 408, 164, 1524, 189, 1541, 1306, 419, 1310); // 4
    //SDL_Surface* newImage = crop(image, 625, 179, 1367, 694, 849, 1434, 110, 915); // 5
    //SDL_Surface* newImage = crop(image, 64, 52, 1959, 54, 2102, 1848, 24, 1932); // 6
    IMG_SavePNG(newImage, "RESULT.png");
	  SDL_FreeSurface(newImage);
    return 0;
}*/
