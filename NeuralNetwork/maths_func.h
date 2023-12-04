#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>


// random number generator (between 0 and 1)
double randomDbl();

// sigmoid function
double sigmoid(double x);

// derivative of sigmoid function
double sigmoid_prime(double x);

// randomize array
void shuffle(size_t* array, size_t n);

// relu function
double relu(double x);

// derivative of relu function
double relu_prime(double x);

// softmax function
void softmax(double *input, double *output, size_t size);

