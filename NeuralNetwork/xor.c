#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/* global variables declarations */

// hidden weigths
double w11;
double w12;
double w21;
double w22;

// output weigths
double o1;
double o2;
	
// hidden and output biases
double h1b;
double h2b;
double ob;
	
// Training dataset
char inputs[4][2] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 }
};
char outputs[4] = { 0, 1, 1, 0 };


// random number generator (between 0 and 1)
double random()
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
void shuffle(char s[], size_t n)
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
double predict(double i1, double i2)
{
	double n1 = w11 * i1 + w21 * i2 + h1b;
	n1 = sigmoid(n1);
	double n2 = w12 * i1 + w22 * i2 + h2b;
	n2 = sigmoid(n2);

	double output = n1 * o1 + n2 * o2 + ob;
	output = sigmoid(output);

	return output;
}


// train neural network
void train(long epochs, double lr)
{
	for (size_t i = 1; i < epochs + 1; i++)
	{
		char indexes[4] = { 0, 1, 2, 3 };
		shuffle(indexes, 4);

		for (size_t j = 0; j < 4; j++)
		{
			// get training set and target output
			double i1 = inputs[indexes[j]][0];
			double i2 = inputs[indexes[j]][1];
			double target = outputs[indexes[j]];


			// feed forward
			double n1 = w11 * i1 + w21 * i2 + h1b;
			n1 = sigmoid(n1);
			double n2 = w12 * i1 + w22 * i2 + h2b;
			n2 = sigmoid(n2);

			double output = n1 * o1 + n2 * o2 + ob;
			output = sigmoid(output);


			// backpropagation
			double error = target - output;
			double derror = error * sigmoid_prime(output);

			double dn1 = derror * o1 * sigmoid_prime(n1);
			double dn2 = derror * o2 * sigmoid_prime(n2);

			o1 += n1 * derror * lr;
			o2 += n2 * derror * lr;
			ob += derror * lr;

			w11 += i1 * dn1 * lr;
			w21 += i2 * dn1 * lr;
			h1b += dn1 * lr;
			w12 += i1 * dn2 * lr;
			w22 += i2 * dn2 * lr;
			h2b += dn2 * lr;
		}

		if (i % 1000 == 0)
		{
			printf("RESULTS FOR EPOCH %li\n", i);
			for (size_t j = 0; j < 4; j++)
			{
				double o = predict(inputs[j][0], inputs[j][1]);
				printf("For input [%hhi, %hhi] expected %hhi, predicted %f\n",
						inputs[j][0], inputs[j][1], outputs[j], o);
			}
			printf("\n");
		}
	}
}


int main()
{
	/* global variables definitions */

	// hidden weigths
	w11 = random();
	w12 = random();
	w21 = random();
	w22 = random();

	// output weigths
	o1 = random();
	o2 = random();
	
	// hidden and output biases
	h1b = 0;
	h2b = 0;
	ob = 0;
	
	train(50000, 0.1f);

	return 0;
}
