#include "resultImage.h"

static void drawGrid(SDL_Renderer *renderer) 
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int k = 0;
    for (int i = 0; i <= SCREEN_WIDTH; i += CELL_SIZE) 
    { 
        
        if (k%3 == 0)
        {
            if(i!=SCREEN_WIDTH)
                SDL_RenderDrawLine(renderer, i+1, 0, i+1, SCREEN_HEIGHT);
            if(i!=0)
                SDL_RenderDrawLine(renderer, i-1, 0, i-1, SCREEN_HEIGHT);
        }
        
        SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT);
        k++;
    }
    k=0;
    for (int j = 0; j <= SCREEN_HEIGHT; j += CELL_SIZE) 
    {
        if (k%3 == 0)
        {
            if(j!=0)
            {
                SDL_RenderDrawLine(renderer, 0, j-1, SCREEN_WIDTH, j-1);
            }
            if(j!=SCREEN_HEIGHT)
            {
                SDL_RenderDrawLine(renderer, 0, j+1, SCREEN_WIDTH, j+1);
            }
        }
        
        SDL_RenderDrawLine(renderer, 0, j, SCREEN_WIDTH, j);
        k++;
    }
}

static void savePNG(SDL_Renderer *renderer,char* name) 
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (surface == NULL) 
    {
        printf("Failed to create surface! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, name);

    SDL_FreeSurface(surface);
}

static void drawNumber(SDL_Renderer *renderer, int row, int col, int num,SDL_Color color) 
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;

    char text[2];
    sprintf(text, "%d", num);

    TTF_Font *font = TTF_OpenFont(/*"arial.ttf"*/"Image/SolvedImage/arial.ttf", 28); 
    if (!font) 
    {
        printf("Error getting the font : %s\n", TTF_GetError());
        return;
    }

    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    rect.x = col * CELL_SIZE + CELL_SIZE / 3;
    rect.y = row * CELL_SIZE + CELL_SIZE / 4;
    rect.w = CELL_SIZE / 2;
    rect.h = CELL_SIZE / 2;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

static void drawSudoku(SDL_Renderer *renderer, int sudoku[GRID_SIZE][GRID_SIZE],int sudokubase[GRID_SIZE][GRID_SIZE]) 
{
    drawGrid(renderer);

    SDL_Color color1 = {0, 0, 0, 255}; 
    SDL_Color color2 = {255 , 0, 0, 255};

    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        for (int j = 0; j < GRID_SIZE; ++j) 
        {
            if (sudoku[i][j] != 0) 
            {
                if (sudokubase[i][j])
                    drawNumber(renderer, i, j, sudoku[i][j],color1);
                else
                    drawNumber(renderer, i, j, sudoku[i][j],color2);
            }   
        }
    }
}

static int getMatriceSolved(int sudoku[GRID_SIZE][GRID_SIZE],char** argv)
{
    char *filename = argv[2];
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    char ch;
    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        for (int j = 0; j < GRID_SIZE; ++j) 
        {
            while ((ch = fgetc(fp)) != EOF && (ch < '0'|| ch>'9'))
            {}
            sudoku[i][j]=ch-'0';

        }
    }
    

    fclose(fp);

    return 1;
}

static int getMatriceBase(int sudoku[GRID_SIZE][GRID_SIZE],char** argv)
{
    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    char ch;
    for (int i = 0; i < GRID_SIZE; ++i) 
    {
        for (int j = 0; j < GRID_SIZE; ++j) 
        {
            while ((ch = fgetc(fp)) != EOF && (ch < '0'|| ch>'9') && ch!= '.')
            {}
            if (ch=='.')
                sudoku[i][j]=0;
            else
                sudoku[i][j]=ch-'0';
        }
    }
    
    fclose(fp);

    return 1;
}

int main(int argc, char** argv) 
{
    if (argc!= 3 && argc != 2)
    {
        errx(EXIT_FAILURE,"Use : ./result [solved grid] [base grid]");
    }


    if (TTF_Init() < 0) 
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }
    int sudoku[GRID_SIZE][GRID_SIZE] = {0};
    if (argc==3)
    {
        getMatriceSolved(sudoku,argv);
    }
    int sudokubase[GRID_SIZE][GRID_SIZE] ={0};
    getMatriceBase(sudokubase,argv);
    

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    window = SDL_CreateWindow("Sudoku Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN);

    if (window == NULL) 
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) 
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }


    drawSudoku(renderer,sudokubase,sudokubase);
    savePNG(renderer,"gridbefore.png");
    if (argc==3)
    {
        drawSudoku(renderer, sudoku, sudokubase);
        savePNG(renderer,"gridresult.png");
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    
    SDL_Quit();
    TTF_Quit();


    return 0;
}
