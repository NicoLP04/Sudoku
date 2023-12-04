#include "maths_func.h"


// random number generator (between -0.5 and 0.5)
double randomDbl()
{
	return (((double)rand()) / ((double)RAND_MAX)) - 0.5f;
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

void shuffle(size_t *array, size_t n)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);


    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));
            size_t t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
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
void softmax(double *input, double *output, size_t size)
{
	double sum = 0;
	for (size_t k = 0; k < size; k++)
	{
		output[k] = exp(input[k]);
		sum += output[k];
	}

	for (size_t k = 0; k < size; k++)
	{
		output[k] /= sum;
	}
}

