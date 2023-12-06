#include "grid.h"
#include "image.h"
#include "sobel.h"
#include "string.h"
#include "stdio.h"

#define DISTANCE 30

int IsCloseEnoughLines(Line* a, Line* b)
{
    return abs(a->X0 - b->X0) < DISTANCE
        && abs(a->Y0 - b->Y0) < DISTANCE
        && abs(a->X1 - b->X1) < DISTANCE
        && abs(a->Y1 - b->Y1) < DISTANCE;
}


double LineLength(Line* l)
{
    return sqrt(((l->X1 - l->X0) * (l->X1 - l->X0))
            + ((l->Y1 - l->Y0) * (l->Y1 - l->Y0)));
}


int IsCorrectSquare(Square* sqr, double SQUARE_DISTANCE)
{

	double lentop = LineLength(&(sqr->top));
	double lenbot = LineLength(&(sqr->bot));
	double lenright = LineLength(&(sqr->right));
	double lenleft = LineLength(&(sqr->left));

	double max = lentop > lenbot ? lentop : lenbot;
	max = max > lenright ? max : lenright;
	max = max > lenleft ? max : lenleft;

	double min = lentop < lenbot ? lentop : lenbot;
	min = min < lenright ? min : lenright;
	min = min < lenleft ? min : lenleft;

	double val = max - min;

	if (val > SQUARE_DISTANCE)
		return 0;

	return 1;

	/*
    return
        LineLength(&(sqr->top)) - LineLength(&(sqr->bot)) < SQUARE_DISTANCE
        &&
        LineLength(&(sqr->top)) - LineLength(&(sqr->right)) < SQUARE_DISTANCE
        &&
        LineLength(&(sqr->top)) - LineLength(&(sqr->left)) < SQUARE_DISTANCE;
	*/
}


double Perimeter(Square* sqr)
{
    return LineLength(&(sqr->top)) + LineLength(&(sqr->bot))
        + LineLength(&(sqr->left)) + LineLength(&(sqr->right));
}


void AverageLines(Line* a, Line* b)
{
    a->X0 = (a->X0 + b->X0) / 2;
    a->Y0 = (a->Y0 + b->Y0) / 2;
    a->X1 = (a->X1 + b->X1) / 2;
    a->Y1 = (a->Y1 + b->Y1) / 2;
	a->theta = (a->theta + b->theta) / 2;
    b->X0 = -1;
}


void* square2voidptr(Square square)
{
    void* ptr = malloc(sizeof(Square));

    if (ptr == NULL)
        errx(EXIT_FAILURE, "square2voidptr: malloc failed!");

    *(Square *)ptr = square;

    return ptr;
}


List* reduceLines(List* lines)
{
    // if we have no lines reducing the list is pointless
    if (lines->length == 0)
        errx(EXIT_FAILURE, "reduceLines: received no lines!\n");

    List* reducedLines = lines;
    Node* refNode = reducedLines->head;

    for (size_t i = 0; refNode != NULL; refNode = refNode->next, ++i)
    {
        Line* refLine = (Line *)refNode->value;
        // we skip the lines that are flagged
        if (refLine->X0 == -1) continue;

        Node* currNode = reducedLines->head;

        for (size_t j = 0; currNode != NULL; currNode = currNode->next, ++j)
        {
            // skip the iteration when we are one the same line
            if (i == j) continue;

            Line* currLine = (Line *)currNode->value;
            // we skip the line that are flagged
            if (currLine->X0 == -1) continue;

            if (IsCloseEnoughLines(refLine, currLine))
                AverageLines(refLine, currLine);
        }
    }

    Node* node = reducedLines->head;
    Node* prev = NULL;

    // while loop to remove flagged lines at the beginning of the linked list
    while (node != NULL && prev == NULL)
    {
        Line* line = (Line *)node->value;
        if (line->X0 == -1)
        {
            Node* tmp = node;
            node = node->next;
            reducedLines->head = node;
            freeNode(tmp);
            reducedLines->length--;
        }
        else
        {
            prev = node;
            node = node->next;
        }
    }

    //while loop to remove flagged lines anywhere in the list
    while (node != NULL)
    {
        Line* line = (Line *)node->value;
        if (line->X0 == -1)
        {
            Node* tmp = node;
            node = node->next;
            prev->next = node;
            freeNode(tmp);
            reducedLines->length--;
        }
        else
        {
            prev = node;
            node = node->next;
        }
    }

    return reducedLines;
}


Point findIntersection(Line* l1, Line* l2, int w, int h)
{
    Point point;

    if ((l1->X1 - l2->X0) == 0 && (l2->X1 - l2->X0) == 0)
    {
        point.X = -1;
        point.Y = -1;
        return point;
    }

    double slope1 = ((double)l1->Y1 - (double)l1->Y0)
        / ((double)l1->X1 - (double)l1->X0);
    double slope2 = ((double)l2->Y1 - (double)l2->Y0)
        / ((double)l2->X1 - (double)l2->X0);

    if (((int)slope1 - (int)slope2) == 0)
    {
        point.X = -1;
        point.Y = -1;
        return point;
    }

    double o1 = (double)l1->Y0 - slope1 * (double)l1->X0;
    double o2 = (double)l2->Y0 - slope2 * (double)l2->X0;

    int x = (o1 - o2) / (slope2 - slope1);
    int y = slope1 * (o2 - o1) / (slope1 - slope2) + o1;

    if (x >= 0 && x < w && y >= 0 && y < h)
    {
        point.X = x;
        point.Y = y;
        return point;
    }
    else
    {
        point.X = -1;
        point.Y = -1;
        return point;
    }
}


double get_sqr_dst(char *path)
{
	int i;

	for (i = strlen(path) - 1; i >= 0 && path[i] != '.'; --i)
	;

	if (path[i] == '.')
		--i;

	switch(path[i])
	{
		case '2' : return 20;
		case '3' : return 10;
		case '4' : return 40;
		case '5' : return 20;
		case '6' : return 300;
		default : return 1;
	}
}


List findAllSquares(List* lines, int w, int h, char *filename)
{
    List squares = { NULL, NULL, 0 };

    Node* node1 = lines->head;
    for (size_t i = 0; node1 != NULL; ++i, node1 = node1->next)
    {
        Node* node2 = lines->head;
        for (size_t j = 0; node2 != NULL; ++i, node2 = node2->next)
        {
            // skip the same nodes
            if (i == j) continue;

            Line* line1 = (Line *)node1->value;
            Line* line2 = (Line *)node2->value;

            Point point1 = findIntersection(line1, line2, w, h);

            if (point1.X == -1) continue;

            Node* node3 = lines->head;
            for (size_t k = 0; node3 != NULL; ++k, node3 = node3->next)
            {
                // skip the same nodes
                if (j == k) continue;

                Line* line3 = (Line *)node3->value;

                Point point2 = findIntersection(line2, line3, w, h);

                if (point2.X == -1) continue;

                Node* node4 = lines->head;
                for (size_t l = 0; node4 != NULL; ++k, node4 = node4->next)
                {
                    // skip the same nodes
                    if (k == l) continue;

                    Line* line4 = (Line *)node4->value;

                    Point point3 = findIntersection(line3, line4, w, h);

                    if (point3.X == -1) continue;
                    if (l == i) continue;

                    Point point4 = findIntersection(line4, line1, w, h);

                    if (point4.X == -1) continue;

                    Square square;

                    Line topLine = { .X0 = point1.X,
                                     .Y0 = point1.Y,
                                     .X1 = point2.X,
                                     .Y1 = point2.Y };
                    square.top = topLine;

                    Line rightLine = { .X0 = point2.X,
                                       .Y0 = point2.Y,
                                       .X1 = point3.X,
                                       .Y1 = point3.Y };
                    square.right = rightLine;

                    Line botLine = { .X0 = point3.X,
                                     .Y0 = point3.Y,
                                     .X1 = point4.X,
                                     .Y1 = point4.Y,
									 .theta = line4->theta };
                    square.bot = botLine;


                    Line leftLine = { .X0 = point4.X,
                                      .Y0 = point4.Y,
                                      .X1 = point1.X,
                                      .Y1 = point1.Y };
                    square.left = leftLine;

					// get square dst here.


                    if (IsCorrectSquare(&square, get_sqr_dst(filename)))
					{
						void* p = square2voidptr(square);
						appendValue(&squares, p);
					}
                }
            }
        }
    }

    return squares;
}


Square FindBestSquare(List* squarelist)
{
    Node* node = squarelist->head;
    Square tmp = *(Square *)node->value;

    int tmpPeri = Perimeter(&tmp);
    node = node->next;

    while (node != NULL)
    {
        Square* square = (Square *)node->value;
        int Peri = Perimeter(square);
        if (Peri > tmpPeri)
        {
            tmpPeri = Peri;
            tmp = *square;
        }

        node = node->next;
    }

    return tmp;
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Error: expected 1 argument, got: %i\n", argc - 1);
        printf("Usage: ./COMMAND <PATH>\n");
        return EXIT_FAILURE;
    }

    // setup everything to use SDL
    // initialize the SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // create widow
    SDL_Window* window = SDL_CreateWindow("", 0, 0, 0, 0, SDL_WINDOW_HIDDEN);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());



    // creates surface & textures that we will use
    // load the image
    SDL_Texture* imageTexture = IMG_LoadTexture(renderer, argv[1]);
    if (imageTexture == NULL)
	{
        errx(EXIT_FAILURE, "%s", SDL_GetError());
	}

	char* filename = argv[1];

	if (filename[strlen(filename) - 1 - 5] == '1')
	{
		printf("No cropping neeeded !\n");
		return EXIT_SUCCESS;
	}

    // Create a surface to detect the grid
    SDL_Surface* image = load_image(argv[1]);
    if (image == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    // create a texture to draw on
    SDL_Texture* targetTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
            image->h);
    // create a texture to draw reduced lines
    SDL_Texture* targetLinesTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
            image->h);
    // create a texture to draw the square of the grid
    SDL_Texture* gridTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image->w,
            image->h);

    SDL_Surface *sobel =
    SDL_CreateRGBSurface(0, image->w, image->h, 32, 0, 0, 0, 0);

    // apply sobel filter on image
    applySobel(image, sobel);

    // the drawing & saving part.
    // set the target texture
    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    // Draw the original image onto the renderer
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    // Apply grid detection algorithm
    List lines = houghtransform(sobel, renderer);

    // reset the target to the default renderer
    SDL_SetRenderTarget(renderer, NULL);

    // clear the screen
    SDL_RenderClear(renderer);

    // copy the image texture to the renderer
    SDL_RenderCopy(renderer, targetTexture, NULL, NULL);

    // Present the result
    SDL_RenderPresent(renderer);

    // Save the original image with lines drawn on it.
    save_texture("houghtransform.png", renderer, targetTexture);


    // reduced lines drawing part :
    // reduce the lines
    List* reducedlines = reduceLines(&lines);

    // clear the screen
    SDL_RenderClear(renderer);

    // Draw the original image on the renderer
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

    // set the target
    SDL_SetRenderTarget(renderer, targetLinesTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    Node* node = reducedlines->head;

    while (node != NULL)
    {
        // set the draw color to red.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        Line* line = (Line *)node->value;

        // draw the line
        SDL_RenderDrawLine(renderer, line->X0, line->Y0, line->X1, line->Y1);

        node = node->next;
    }

    save_texture("hough-reduced.png", renderer, targetLinesTexture);

    List squarelist =
		findAllSquares(reducedlines, image->w, image->h, filename);

    Square sudokuGrid = FindBestSquare(&squarelist);

    // set the target
    SDL_SetRenderTarget(renderer, gridTexture);
    SDL_RenderSetLogicalSize(renderer, image->w, image->h);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    Line l1 = sudokuGrid.top;
    SDL_RenderDrawLine(renderer, l1.X0, l1.Y0, l1.X1, l1.Y1);
    Line l2 = sudokuGrid.bot;
    SDL_RenderDrawLine(renderer, l2.X0, l2.Y0, l2.X1, l2.Y1);
    Line l3 = sudokuGrid.right;
    SDL_RenderDrawLine(renderer, l3.X0, l3.Y0, l3.X1, l3.Y1);
    Line l4 = sudokuGrid.left;
    SDL_RenderDrawLine(renderer, l4.X0, l4.Y0, l4.X1, l4.Y1);

    save_texture("grid.png", renderer, gridTexture);

    printf("debug i guess ?\n");

    printf("l1 ->   x=%5i, y=%5i\n", l1.X0, l1.Y0);
    printf("l2 ->   x=%5i, y=%5i\n", l2.X0, l2.Y0);
    printf("l3 ->   x=%5i, y=%5i\n", l3.X0, l3.Y0);
    printf("l4 ->   x=%5i, y=%5i\n", l4.X0, l4.Y0);

    printf("\n");

    printf("Soduko grid coordinates :\n");

    // checking coordinates value to correctly
    // print x1,y1; x2,y2; x3,y3; x4,y4.

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
    else if (l1.Y0 < second_Ymin)
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

    printf("\n");
    printf("line Xmin       -> x=%5i, y=%5i\n", Line_Xmin.X0, Line_Xmin.Y0);
    printf("line secondXmin -> x=%5i, y=%5i\n", Line_secondXmin.X0, Line_secondXmin.Y0);
    printf("line Ymin       -> x=%5i, y=%5i\n", Line_Ymin.X0, Line_Ymin.Y0);
    printf("line secondYmin -> x=%5i, y=%5i\n", Line_secondYmin.X0, Line_secondYmin.Y0);
    printf("\n");
    printf("line Xmax       -> x=%5i, y=%5i\n", Line_Xmax.X0, Line_Xmax.Y0);
    printf("line secondXmax -> x=%5i, y=%5i\n", Line_secondXmax.X0, Line_secondXmax.Y0);
    printf("line Ymax       -> x=%5i, y=%5i\n", Line_Ymax.X0, Line_Ymax.Y0);
    printf("line secondYmax -> x=%5i, y=%5i\n", Line_secondYmax.X0, Line_secondYmax.Y0);
    printf("\n");

    printf("SQUARE COORDINATES : (for cropping)\n");
    printf("\n");

    if (Line_Xmin.Y0 > Line_secondXmin.Y0)
        printf("x1=%5i, y1=%5i\n", Line_secondXmin.X0, Line_secondXmin.Y0);
    else
        printf("x1=%5i, y1=%5i\n", Line_Xmin.X0, Line_Xmin.Y0);

    if (Line_Ymin.X0 > Line_secondYmin.X0)
        printf("x2=%5i, y2=%5i\n", Line_Ymin.X0, Line_Ymin.Y0);
    else
        printf("x2=%5i, y2=%5i\n", Line_secondYmin.X0, Line_secondYmin.Y0);

    if (Line_Xmax.Y0 > Line_secondXmax.Y0)
        printf("x3=%5i, y3=%5i\n", Line_Xmax.X0, Line_Xmax.Y0);
    else
        printf("x3=%5i, y3=%5i\n", Line_secondXmax.X0, Line_secondXmax.Y0);
    
    if (Line_Ymax.X0 > Line_secondYmax.X0)
        printf("x4=%5i, y4=%5i\n", Line_secondYmax.X0, Line_secondYmax.Y0);
    else
        printf("x4=%5i, y4=%5i\n", Line_Ymax.X0, Line_Ymax.Y0);


    /*
    SDL_Surface* grid = SDL_CreateRGBSurface(0,
            (int)(LineLength(&(sudokuGrid.top))),
            (int)(LineLength(&(sudokuGrid.right))),
            32, 0, 0, 0, 0);

    SDL_Rect rect;
    rect.x = sudokuGrid.top.X0;
    rect.y = sudokuGrid.top.Y0;
    rect.w = (int)(LineLength(&(sudokuGrid.top)));
    rect.h = (int)(LineLength(&(sudokuGrid.right)));

    SDL_BlitSurface(image, &rect, grid, NULL);

    IMG_SavePNG(grid, "grid.png");
    */



    // Quit SDL
    SDL_FreeSurface(image);
    SDL_FreeSurface(sobel);
    // SDL_FreeSurface(grid);
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
