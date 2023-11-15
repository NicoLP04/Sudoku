#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include "neural_network.h"
#include "maths_func.h"


void exit_help()
{
	char help[] =
		"Usage:\n"
		"--predict\n"
		"--train\n"
		"--reset\n";

	errx(EXIT_FAILURE, "%s", help);
}


// main function
int main(int argc, char **argv)
{
	if (argc == 1)
		exit_help();

	srand(time(NULL));

	if (strcmp(argv[1], "--predict") == 0)
	{
		if (argc > 2)
			exit_help();
		print_results("values");
	}
	else if (strcmp(argv[1], "--train") == 0)
	{	
		if (argc > 4)
			exit_help();

		int epochs = 10;
		double lr = 0.1f;
		char *endptr;
	
		if (argc >= 3)
			epochs = atoi(argv[2]);
		if (argc == 4)
		{
			lr = strtod(argv[3], &endptr);
			if (*endptr != 0)
				exit_help();
		}
	
		train(epochs, lr, "values", 9, 9);
	
		printf("Neural network successfully trained with parameters: \n");
		printf("epochs = %d && lr = %f\n", epochs, lr);
	}
	else if (strcmp(argv[1], "--reset") == 0)
	{
		if (argc > 2)
			exit_help();
		reset("values");
	}
	else
		exit_help();


	return EXIT_SUCCESS;
}
