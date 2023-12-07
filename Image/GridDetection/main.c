#include "grid.h"
#include "houghtransform.h"
#include "image.h"
#include "linked_list.h"
#include "pixel.h"
#include "sobel.h"



// main function :
// does the full process of grid detection.
// step 1 apply sobel on the image
// step 2 calculate houghspace from sobel image
// step 3 reduce lines detected by houghtransform
// step 4 detect biggest square representing the grid
// step 5 re-order square corners coordinates
// step 6 crop the soduko grid
// FINAL STEP -> save image


int main(int argc, char** argv)
{
	// arguments parsing.
	if (argc != 2)
	{
		printf("Error: expected 1 argument, got: %i\n", argc - 1);
		printf("Usage: ./COMMAND <PATH>\n");
		return EXIT_FAILURE;
	}

	// -----------------------------------------------------------------------
	// setup the SDL environnement.
	// SDL initialisation.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	// window.
	SDL_Window* window = SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN);
	if (window == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	// renderer.
	SDL_Renderer* renderer =
		SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		errx(EXIT_FAILURE, "%s", SDL_GetError());


	// -----------------------------------------------------------------------
	// creating surfaces & textures that will be used to load the image and
	// draw the results for visiualization.

	// load the input image as a texture.
	SDL_Texture* imageTexture = IMG_LoadTexture(renderer, argv[1]);
    if (imageTexture == NULL)
	{
		errx(EXIT_FAILURE, "%s", SDL_GetError());
	}

	// specific case if input image is "image_1.jpeg".
	char* filename = argv[1];
	if (filename[strlen(filename) - 1 - 5] == '1')
	{
		printf("No cropping neeeded !\n");
		return EXIT_SUCCESS;
	}

	// load the input image as a surface to fill the hough space.
	SDL_Surface* image = load_image(argv[1]);
    if (image == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

	// texture to draw detected lines.
    SDL_Texture* targetTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
            image->h);

	// texture to draw reduced lines.
	SDL_Texture* targetLinesTexture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
		image->h);

	// texture to draw sudoku grid.
    SDL_Texture* gridTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
            image->h);

	// -----------------------------------------------------------------------
	// applying sobel filter on the input image.
	SDL_Surface *sobel =
    	SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);

	applySobel(image, sobel);


	// -----------------------------------------------------------------------
	// full process of the sudoku grid detection.
	// drawing & saving the images for visualization.

    // set the target texture.
    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    // draw the original image onto the renderer.
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    // apply grid detection algorithm.
    List lines = houghtransform(sobel, renderer);

    // reset the target to the default renderer.
    SDL_SetRenderTarget(renderer, NULL);

    // clear the screen.
    SDL_RenderClear(renderer);

    // copy the image texture to the renderer.
    SDL_RenderCopy(renderer, targetTexture, NULL, NULL);

    // present the result.
    SDL_RenderPresent(renderer);

    // save the original image with lines drawn on it.
    save_texture("houghtransform.png", renderer, targetTexture);

    // reducing the lines.
    List* reducedlines = reduceLines(&lines);

    // clear the screen.
    SDL_RenderClear(renderer);

    // draw the original image on the renderer.
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    // set the target.
    SDL_SetRenderTarget(renderer, targetLinesTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    Node* node = reducedlines->head;

    while (node != NULL)
    {
        // set the draw color to red.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        Line* line = (Line *)node->value;

        // draw the line.
        SDL_RenderDrawLine(renderer, line->X0, line->Y0, line->X1, line->Y1);

        node = node->next;
    }

    save_texture("hough-reduced.png", renderer, targetLinesTexture);

    List squarelist =
		findAllSquares(reducedlines, image->w, image->h, filename);

    Square sudokuGrid = FindBestSquare(&squarelist);

    // set the target.
    SDL_SetRenderTarget(renderer, gridTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

	// draw the square.
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    Line l1 = sudokuGrid.top;
    SDL_RenderDrawLine(renderer, l1.X0, l1.Y0, l1.X1, l1.Y1);
    Line l2 = sudokuGrid.bot;
    SDL_RenderDrawLine(renderer, l2.X0, l2.Y0, l2.X1, l2.Y1);
    Line l3 = sudokuGrid.right;
    SDL_RenderDrawLine(renderer, l3.X0, l3.Y0, l3.X1, l3.Y1);
    Line l4 = sudokuGrid.left;
    SDL_RenderDrawLine(renderer, l4.X0, l4.Y0, l4.X1, l4.Y1);

    save_texture("hough-grid.png", renderer, gridTexture);


	// -----------------------------------------------------------------------
	// logic to reorganize coordinates of the grid corners to correctly
	// call crop function.

	// checking coordinates value to correctly
    // have x1,y1; x2,y2; x3,y3; x4,y4.

    // XMIN
    Line Line_Xmin = l1;
    if (Line_Xmin.X0 > l2.X0)
        Line_Xmin = l2;
    if (Line_Xmin.X0 > l3.X0)
        Line_Xmin = l3;
    if (Line_Xmin.X0 > l4.X0)
        Line_Xmin = l4;

    Line Line_secondXmin = { .X0 = 0,
                             .Y0 = 0,
                             .X1 = 0,
                             .Y1 = 0 };
    int second_Xmin = INT32_MAX;
    if (l1.X0 > Line_Xmin.X0)
    {
        second_Xmin = l1.X0;
        Line_secondXmin = l1;
    }
    else if (l1.X0 < second_Xmin)
    {
        second_Xmin = l1.X0;
        Line_secondXmin = l1;
    }
    if (l2.X0 > Line_Xmin.X0 && l2.X0 < second_Xmin)
    {
        second_Xmin = l2.X0;
        Line_secondXmin = l2;
    }
    if (l3.X0 > Line_Xmin.X0 && l3.X0 < second_Xmin)
    {
        second_Xmin = l3.X0;
        Line_secondXmin = l3;
    }
    if (l4.X0 > Line_Xmin.X0 && l4.X0 < second_Xmin)
    {
        second_Xmin = l4.X0;
        Line_secondXmin = l4;
    }

    // YMIN
    Line Line_Ymin = l1;

    if (Line_Ymin.Y0 > l2.Y0)
        Line_Ymin = l2;
    if (Line_Ymin.Y0 > l3.Y0)
        Line_Ymin = l3;
    if (Line_Ymin.Y0 > l4.Y0)
        Line_Ymin = l4;

    Line Line_secondYmin = { .X0 = 0,
                             .Y0 = 0,
                             .X1 = 0,
                             .Y1 = 0 };
    int second_Ymin = INT32_MAX;
    if (l1.Y0 > Line_Ymin.Y0 && l1.Y0 != Line_Ymin.Y0)
    {
        second_Ymin = l1.Y0;
        Line_secondYmin = l1;
    }
    else if (l1.Y0 < second_Ymin && l1.Y0 != Line_Ymin.Y0)
    {
        second_Ymin = l1.Y0;
        Line_secondYmin = l1;
    }

    if (l2.Y0 > Line_Ymin.Y0 && l2.Y0 < second_Ymin)
    {
        second_Ymin = l2.Y0;
        Line_secondYmin = l2;
    }

    if (l3.Y0 > Line_Ymin.Y0 && l3.Y0 < second_Ymin)
    {
        second_Ymin = l3.Y0;
        Line_secondYmin = l3;
    }

    if (l4.Y0 > Line_Ymin.Y0 && l4.Y0 < second_Ymin)
    {
        second_Ymin = l4.Y0;
        Line_secondYmin = l4;
    }

    // XMAX
    Line Line_Xmax = l1;
    if (Line_Xmax.X0 < l2.X0)
        Line_Xmax = l2;
    if (Line_Xmax.X0 < l3.X0)
        Line_Xmax = l3;
    if (Line_Xmax.X0 < l4.X0)
        Line_Xmax = l4;
    Line Line_secondXmax = { .X0 = 0, .Y0 = 0, .X1 = 0, .Y1 = 0 };

    if (l1.X0 != Line_Xmax.X0 && l1.Y0 != Line_Xmax.Y0 &&
        l1.X0 != Line_Xmin.X0 && l1.Y0 != Line_Xmin.Y0 &&
        l1.X0 != Line_secondXmin.X0 && l1.Y0 != Line_secondXmin.Y0)
    {
        Line_secondXmax = l1;
    }

    if (l2.X0 != Line_Xmax.X0 && l2.Y0 != Line_Xmax.Y0 &&
        l2.X0 != Line_Xmin.X0 && l2.Y0 != Line_Xmin.Y0 &&
        l2.X0 != Line_secondXmin.X0 && l2.Y0 != Line_secondXmin.Y0)
    {
        Line_secondXmax = l2;
    }

    if (l3.X0 != Line_Xmax.X0 && l3.Y0 != Line_Xmax.Y0 &&
        l3.X0 != Line_Xmin.X0 && l3.Y0 != Line_Xmin.Y0 &&
        l3.X0 != Line_secondXmin.X0 && l3.Y0 != Line_secondXmin.Y0)
    {
        Line_secondXmax = l3;
    }

    if (l4.X0 != Line_Xmax.X0 && l4.Y0 != Line_Xmax.Y0 &&
        l4.X0 != Line_Xmin.X0 && l4.Y0 != Line_Xmin.Y0 &&
        l4.X0 != Line_secondXmin.X0 && l4.Y0 != Line_secondXmin.Y0)
    {
        Line_secondXmax = l4;
    }

    // YMAX
    Line Line_Ymax = l1;

    if (Line_Ymax.Y0 < l2.Y0)
        Line_Ymax = l2;
    if (Line_Ymax.Y0 < l3.Y0)
        Line_Ymax = l3;
    if (Line_Ymax.Y0 < l4.Y0)
        Line_Ymax = l4;
    Line Line_secondYmax = { .X0 = 0, .Y0 = 0, .X1 = 0, .Y1 = 0 };

    if (l1.X0 != Line_Ymax.X0 && l1.Y0 != Line_Ymax.Y0 &&
        l1.X0 != Line_Ymin.X0 && l1.Y0 != Line_Ymin.Y0 &&
        l1.X0 != Line_secondYmin.X0 && l1.Y0 != Line_secondYmin.Y0)
    {
        Line_secondYmax = l1;
    }

    if (l2.X0 != Line_Ymax.X0 && l2.Y0 != Line_Ymax.Y0 &&
        l2.X0 != Line_Ymin.X0 && l2.Y0 != Line_Ymin.Y0 &&
        l2.X0 != Line_secondYmin.X0 && l2.Y0 != Line_secondYmin.Y0)
    {
        Line_secondYmax = l2;
    }

    if (l3.X0 != Line_Ymax.X0 && l3.Y0 != Line_Ymax.Y0 &&
        l3.X0 != Line_Ymin.X0 && l3.Y0 != Line_Ymin.Y0 &&
        l3.X0 != Line_secondYmin.X0 && l3.Y0 != Line_secondYmin.Y0)
    {
        Line_secondYmax = l3;
    }

    if (l4.X0 != Line_Ymax.X0 && l4.Y0 != Line_Ymax.Y0 &&
        l4.X0 != Line_Ymin.X0 && l4.Y0 != Line_Ymin.Y0 &&
        l4.X0 != Line_secondYmin.X0 && l4.Y0 != Line_secondYmin.Y0)
    {
        Line_secondYmax = l4;
    }

    int x1, y1, x2, y2, x3, y3, x4, y4;

    if (Line_Xmin.Y0 > Line_secondXmin.Y0)
    {
        printf("x1=%5i, y1=%5i\n", Line_secondXmin.X0, Line_secondXmin.Y0);
        x1 = Line_secondXmin.X0;
        y1 = Line_secondXmin.Y0;
    }
    else
    {
        printf("x1=%5i, y1=%5i\n", Line_Xmin.X0, Line_Xmin.Y0);
        x1 = Line_Xmin.X0;
        y1 = Line_Xmin.Y0;
    }
    if (Line_Ymin.X0 > Line_secondYmin.X0)
    {
        printf("x2=%5i, y2=%5i\n", Line_Ymin.X0, Line_Ymin.Y0);
        x2 = Line_Ymin.X0;
        y2 = Line_Ymin.Y0;
    }
    else
    {
        printf("x2=%5i, y2=%5i\n", Line_secondYmin.X0, Line_secondYmin.Y0);
        x2 = Line_secondYmin.X0;
        y2 = Line_secondYmin.Y0;
    }
    if (Line_Xmax.Y0 > Line_secondXmax.Y0)
    {
        printf("x3=%5i, y3=%5i\n", Line_Xmax.X0, Line_Xmax.Y0);
        x3 = Line_Xmax.X0;
        y3 = Line_Xmax.Y0;
    }
    else
    {
        printf("x3=%5i, y3=%5i\n", Line_secondXmax.X0, Line_secondXmax.Y0);
        x3 = Line_secondXmax.X0;
        y3 = Line_secondXmax.Y0;
    }
    if (Line_Ymax.X0 > Line_secondYmax.X0)
    {
        printf("x4=%5i, y4=%5i\n", Line_secondYmax.X0, Line_secondYmax.Y0);
        x4 = Line_secondYmax.X0;
        y4 = Line_secondYmax.Y0;
    }
    else
	{
        printf("x4=%5i, y4=%5i\n", Line_Ymax.X0, Line_Ymax.Y0);
        x4 = Line_Ymax.X0;
        y4 = Line_Ymax.Y0;
	}


	// -----------------------------------------------------------------------
	// call to crop function and saving.

    SDL_Surface *grid = crop(image, x1, y1, x2, y2, x3, y3, x4, y4);

    IMG_SavePNG(grid, "grid.png");

    // Quit SDL

    // SDL_FreeSurface(image); no need to call this line already freed in the
    // call of crop function.
    SDL_FreeSurface(sobel);
    SDL_FreeSurface(grid);

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(targetTexture);
    SDL_DestroyTexture(targetLinesTexture);
    SDL_DestroyTexture(gridTexture);

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    freeList(&lines);
    freeList(&squarelist);

	return EXIT_SUCCESS;
}
