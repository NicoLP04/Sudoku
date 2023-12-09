#include "neural_network.h"
#include "maths_func.h"


/* global variables declarations */

// Nodes
double hiddenLayer[numHiddens];
double outputLayer[numOutputs];

// Biases
double hiddenLayerBias[numHiddens];
double outputLayerBias[numOutputs];

// Weights
double hiddenWeights[numInputs][numHiddens];
double outputWeights[numHiddens][numOutputs];


// to delete
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* temp = IMG_Load(path);
    if (temp  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* newsurf = SDL_ConvertSurfaceFormat(temp,
			SDL_PIXELFORMAT_RGB888,0);
    if (newsurf  == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(temp);
    return newsurf;
}

// predict output
int predict(SDL_Surface *image, char *file)
{
	load(file);

	double inputs[numInputs] = { 0 };
	Uint32 *pixels = image->pixels;
	for (int b = 0; b < image->h; b++)
	{
		for (int a = 0; a < image->w; a++)
		{
			Uint32 pixelValue = pixels[b * image->w + a];
			Uint8 r, g, t;
			SDL_GetRGB(pixelValue, image->format, &r, &g, &t);
			inputs[b * image->w + a] = r;
		}
	}

	// Calculate outputs
	for (size_t k = 0; k < numHiddens; k++)
	{
		double activation = hiddenLayerBias[k];
		for (size_t l = 0; l < numInputs; l++)
			activation += hiddenWeights[l][k] * inputs[l];
		hiddenLayer[k] = sigmoid(activation);
	}

	for (size_t k = 0; k < numOutputs; k++)
	{
		double activation = outputLayerBias[k];
		for (size_t l = 0; l < numHiddens; l++)
			activation += outputWeights[l][k] * hiddenLayer[l];
		outputLayer[k] = activation;	//sigmoid(activation);
	}

	softmax(outputLayer, outputLayer, numOutputs);

  int res = 0;
  for (int i = 0; i < numOutputs; i++)
  {
    if (outputLayer[i] > outputLayer[res])
      res = i;
  }
	return res;
}


// train neural network with parameters epochs and lr
void train(long epochs, double lr, char *file, size_t numImages)
{
	init_weights();
	char *setName = "TrainingSet/LearningSet/";

	// Get all the images from the training set of (TrainingSet/)
	char **images = malloc(numImages * sizeof(char*));
	char **temp = images;
	DIR *d;
  	struct dirent *dir;
  	d = opendir(setName);
  	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				*temp = calloc(40, sizeof(char));
				strcat(*temp, setName);
				strcat(*temp, dir->d_name);
				temp++;
			}
    		}
		closedir(d);
	}

	// loop to train the neural network
	for (long i = 1; i < epochs + 1; i++)
	{
		size_t indexes[numImages];
		for (size_t b = 0; b < numImages; b++)
			indexes[b] = b;

		shuffle(indexes, numImages);

		for (size_t j = 0; j < numImages; j++)
		{
			// get training set and target output
      size_t idx = indexes[j];
			char *name = images[idx];
			SDL_Surface *image = load_image(name);

			double inputs[numInputs] = { 0 };
			Uint32 *pixels = image->pixels;
			for (int b = 0; b < image->h; b++)
			{
				for (int a = 0; a < image->w; a++)
				{
					Uint32 pixelValue = pixels[b * image->w + a];
					Uint8 r, g, t;
					SDL_GetRGB(pixelValue, image->format, &r, &g, &t);
					inputs[b * image->w + a] = r;
				}
			}

			double targets[numOutputs] = { 0 };
			targets[*(name+24) - '0'] = 1;

			// feed forward
			for (size_t k = 0; k < numHiddens; k++)
			{
				double activation = hiddenLayerBias[k];
				for (size_t l = 0; l < numInputs; l++)
					activation += hiddenWeights[l][k] * inputs[l];
				hiddenLayer[k] = sigmoid(activation);
			}

			for (size_t k = 0; k < numOutputs; k++)
			{
				double activation = outputLayerBias[k];
				for (size_t l = 0; l < numHiddens; l++)
					activation += outputWeights[l][k] * hiddenLayer[l];
				outputLayer[k] = activation;		//sigmoid(activation);
			}

			softmax(outputLayer, outputLayer, numOutputs);

			// backpropagation
			double derrors[numOutputs];
			for (size_t k = 0; k < numOutputs; k++)
				derrors[k] =
					(targets[k] - outputLayer[k]);

			double dhidden[numHiddens];
			for (size_t k = 0; k < numHiddens; k++)
			{
				double error = 0.0f;
				for (size_t l = 0; l < numOutputs; l++)
					error += derrors[l] * outputWeights[k][l];
				dhidden[k] = error * sigmoid_prime(hiddenLayer[k]);
			}

			// Apply change
			for (size_t k = 0; k < numHiddens; k++)
			{
				hiddenLayerBias[k] += dhidden[k] * lr;
				for (size_t l = 0; l < numInputs; l++)
					hiddenWeights[l][k] += inputs[l] * dhidden[k] * lr;
			}

			for (size_t k = 0; k < numOutputs; k++)
			{
				outputLayerBias[k] += derrors[k] * lr;
				for (size_t l = 0; l < numHiddens; l++)
					outputWeights[l][k] += hiddenLayer[l] * derrors[k] * lr;
			}
			SDL_FreeSurface(image);
		}
	}


	// free
	for (size_t i = 0; i < numImages; i++)
		free(images[i]);
	free(images);

	save(file);
}

// save weights and biases in file
void save(char *file)
{
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
		fprintf(f, "%f|", hiddenLayerBias[i]);
	fprintf(f, "\n");
	for (size_t i = 0; i < numOutputs; i++)
		fprintf(f, "%f|", outputLayerBias[i]);
	fprintf(f, "\n\n");

	// write weights
	for (size_t i = 0; i < numHiddens; i++)
	{
		for (size_t j = 0; j < numInputs; j++)
			fprintf(f, "%f|", hiddenWeights[j][i]);
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
	for (size_t i = 0; i < numOutputs; i++)
	{
		for (size_t j = 0; j < numHiddens; j++)
			fprintf(f, "%f|", outputWeights[j][i]);
		fprintf(f, "\n");
	}

	fclose(f);
}


// load weights and biases from file
void load(char *file)
{
	FILE *f;
	//printf("File inside neural network load : %s\n", file);
	double a;
        f = fopen(file, "r");

	for (size_t k = 0; k < 3; k++)
		fscanf(f, "%lf|", &a);

	// get hidden biases
	for (size_t k = 0; k < numHiddens; k++)
	{
		fscanf(f, "%lf|", &a);
		hiddenLayerBias[k] = a;
	}
	// get output biases
	for (size_t k = 0; k < numOutputs; k++)
	{
		fscanf(f, "%lf|", &a);
		outputLayerBias[k] = a;
	}

	// get weights
	for (size_t k = 0; k < numHiddens; k++)
	{
		for (size_t l = 0; l < numInputs; l++)
		{
			fscanf(f, "%lf|", &a);
			hiddenWeights[l][k] = a;
		}
	}
	for (size_t k = 0; k < numOutputs; k++)
	{
		for (size_t l = 0; l < numHiddens; l++)
		{
			fscanf(f, "%lf|", &a);
			outputWeights[l][k] = a;
		}
	}

        fclose(f);
}


// init weights with random values
void init_weights()
{
	// Init Weights
	for (size_t i = 0; i < numInputs; i++)
		for (size_t j = 0; j < numHiddens; j++)
			hiddenWeights[i][j] = randomDbl();
	for (size_t i = 0; i < numHiddens; i++)
		for (size_t j = 0; j < numOutputs; j++)
			outputWeights[i][j] = randomDbl();
	for (size_t i = 0; i < numHiddens; i++)
		hiddenLayerBias[i] = 0;
	for (size_t i = 0; i < numOutputs; i++)
		outputLayerBias[i] = 0;
}


void print_results()
{
	char *setName = "TrainingSet/TestSet/";
	size_t numImages = 10;

	// Get all the images from the training set of (TrainingSet/)
	char **images = malloc(numImages * sizeof(char*));
	char **temp = images;
	DIR *d;
  	struct dirent *dir;
  	d = opendir(setName);
  	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				*temp = calloc(40, sizeof(char));
				strcat(*temp, setName);
				strcat(*temp, dir->d_name);
				temp++;
			}
    		}
		closedir(d);
	}

  int good = 0;
	for (size_t i = 0; i < numImages; i++)
	{
		char *name = images[i];
		SDL_Surface *image = load_image(name);
		int res = predict(image, "values");
		int exp = *(name+20) - '0';

		printf("For image %s, ", name);
		if (res != 0)
			printf("Expected %d, Predicted %d (%f)\n",
				exp, res, outputLayer[res] - 0.03f);
		else
			printf("Expected %d, Predicted %d (%f)\n",
				exp, res, outputLayer[res]);

    if (exp == res)
      good++;

		SDL_FreeSurface(image);
	}

  printf("%d / %ld\n", good, numImages);
	// free
	for (size_t i = 0; i < numImages; i++)
		free(images[i]);
	free(images);
}


void reset(char *file)
{
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

