#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include "xor.h"
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


void predict_xor(int argc, char *file, char trainingInputs[4][2],
    char trainingOutputs[4][1])
{
	if (argc > 2)
		exit_help();

	for (size_t j = 0; j < 4; j++)
	{
		double o = predict(trainingInputs[j][0],trainingInputs[j][1],file)[0];
		int r = 0;
		if (o > 0.5f)
			r = 1;
		printf("For input [%hhi, %hhi] expected %hhi, predicted %d (%f)\n",
			trainingInputs[j][0], trainingInputs[j][1],
			trainingOutputs[j][0], r, o);
	}
}


void train_xor(int argc, char** argv, char *file, char trainingInputs[4][2],
    char trainingOutputs[4][1])
{
	if (argc > 4)
		exit_help();

	int epochs = 100000;
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

	train(epochs, lr, trainingInputs, trainingOutputs, file);

	printf("Neural network successfully trained with parameters: \n");
	printf("epochs = %d && lr = %f\n", epochs, lr);
}


void reset_xor(int argc, char *file)
{
	if (argc > 2)
		exit_help();

	FILE *f;

	f = fopen(file,"w");

	if(f == NULL)
	{
		printf("Error!");
		exit(1);
	}

	// write number of Nodes I|H|O
	fprintf(f,"%d|%d|%d\n\n", numInputs, numHiddens, numOutputs);

	// write biases
	for (size_t i = 0; i < numHiddens; i++)
		fprintf(f, "%f|", 0.0f);
	fprintf(f, "\n");
	for (size_t i = 0; i < numOutputs; i++)
		fprintf(f, "%f|", 0.0f);
	fprintf(f, "\n\n");

	// write weights
	for (size_t i = 0; i < numHiddens; i++)
	{
		for (size_t j = 0; j < numInputs; j++)
		{
			double val = randomDbl();
			fprintf(f, "%f|", val);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
	for (size_t i = 0; i < numOutputs; i++)
	{
		for (size_t j = 0; j < numHiddens; j++)
		{
			double val = randomDbl();
			fprintf(f, "%f|", val);
		}
		fprintf(f, "\n");
	}

	fclose(f);

	printf("Neural network successfully reset !\n");
}


// main function
int main(int argc, char **argv)
{
	if (argc == 1)
		exit_help();

	srand(time(NULL));

	// Training dataset
	char trainingOutputs[4][1] = { {0}, {1}, {1}, {0} };
	char trainingInputs[4][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 }
	};

	if (strcmp(argv[1], "--predict") == 0)
		predict_xor(argc, "values", trainingInputs, trainingOutputs);
	else if (strcmp(argv[1], "--train") == 0)
		train_xor(argc, argv, "values", trainingInputs, trainingOutputs);
	else if (strcmp(argv[1], "--reset") == 0)
		reset_xor(argc, "values");
	else
		exit_help();


	return EXIT_SUCCESS;
}


