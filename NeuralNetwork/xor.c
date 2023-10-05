#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/* global variables declarations */

// Number of Nodes
#define numInputs 2
#define numHiddens 3
#define numOutputs 1

// Nodes
double hiddenLayer[numHiddens];
double outputLayer[numOutputs];
	
// Biases
double hiddenLayerBias[numHiddens];
double outputLayerBias[numOutputs];

// Weights
double hiddenWeights[numInputs][numHiddens];
double outputWeights[numHiddens][numOutputs];

// Training dataset
char trainingInputs[4][2] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 }
};
char trainingOutputs[][1] = { {0}, {1}, {1}, {0} };


// random number generator (between 0 and 1)
double randomDbl()
{
	return ((double)rand()) / ((double)RAND_MAX);
}


// sigmoid function
double sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}


// derivative of sigmoid function
double sigmoid_prime(double x)
{
	return x * (1 - x);
}


// randomize array
void shuffle(size_t s[], size_t n)
{
	for (size_t i = 0; i < n - 1; i++) 
	{
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
		char t = s[j];
		s[j] = s[i];
		s[i] = t;
	}
}


// predict output
double *predict(char inputs[])
{
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
		outputLayer[k] = sigmoid(activation);
	}
	
	return outputLayer;
}


// train neural network
void train(long epochs, double lr)
{
	for (long i = 1; i < epochs + 1; i++)
	{
		size_t indexes[4] = { 0, 1, 2, 3 };
		shuffle(indexes, 4);

		for (size_t j = 0; j < 4; j++)
		{
			// get training set and target output
			double i1 = trainingInputs[indexes[j]][0];
			double i2 = trainingInputs[indexes[j]][1];
			double t = trainingOutputs[indexes[j]][0];
			double inputs[2] = { i1, i2 };
			double targets[1] = { t };

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
				outputLayer[k] = sigmoid(activation);
			}


			// backpropagation
			double derrors[numOutputs];
			for (size_t k = 0; k < numOutputs; k++)
				derrors[k] = (targets[k] - outputLayer[k]) * sigmoid_prime(outputLayer[k]);
			
			double dhidden[numHiddens];
			for (size_t k = 0; k < numHiddens; k++)
			{
				double error = 0.0f;
				for (size_t l = 0; l < numOutputs; l++)
					error += derrors[l] * outputWeights[k][l];
				dhidden[k] = error * sigmoid_prime(hiddenLayer[k]);
			}

			// Apply change
			for (size_t k = 0; k < numOutputs; k++)
			{
				outputLayerBias[k] += derrors[k] * lr;
				for (size_t l = 0; l < numHiddens; l++)
					outputWeights[l][k] += hiddenLayer[l] * derrors[k] * lr;
			}
			for (size_t k = 0; k < numHiddens; k++)
			{
				hiddenLayerBias[k] += dhidden[k] * lr;
				for (size_t l = 0; l < numInputs; l++)
					hiddenWeights[l][k] += inputs[l] * dhidden[k] * lr;
			}
		}

		if (i % 1000 == 0)
		{
			double cost = 0;
			for (size_t j = 0; j < 4; j++)
			{
				double o = predict(trainingInputs[j])[0];
				cost += (trainingOutputs[j][0] - o) * (trainingOutputs[j][0] - o);
			}
			cost = cost / 4;
			printf("%li means squared error:%f\n", i, cost);
		}
	}
}


int main()
{
	/* Variables Definitions */
	
	// Weights
	for (size_t i = 0; i < numInputs; i++)
		for (size_t j = 0; j < numHiddens; j++)
			hiddenWeights[i][j] = randomDbl();
	
	for (size_t i = 0; i < numInputs; i++)
		for (size_t j = 0; j < numOutputs; j++)
			outputWeights[i][j] = randomDbl();

	// Biases
	for (size_t i = 0; i < numOutputs; i++)
		outputLayerBias[i] = randomDbl();

	
	int Epochs = 15000;	
	double variance = 10.0f;
	
	train(Epochs, variance);

	for (size_t j = 0; j < 4; j++)
	{
		double o = predict(trainingInputs[j])[0];
		printf("For input [%hhi, %hhi] expected %hhi, predicted %f\n",
			trainingInputs[j][0], trainingInputs[j][1], trainingOutputs[j][0], o);
	}

	return 0;
}
