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
