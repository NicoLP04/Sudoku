#include "maths_func.h"


// random number generator (between -1 and 1)
double randomDbl()
{
	return (((double)rand()) / ((double)RAND_MAX)) * 2 - 1;
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


// relu function
double relu(double x)
{
	if (x <= 0)
		return 0;
	else
		return x;
}


// derivative of relu function
double relu_prime(double x)
{
	if (x <= 0)
		return 0;
	else
		return 1;
}


// softmax function
double softmax(double x)
{
	return exp(x) / (1 + exp(x));
}

