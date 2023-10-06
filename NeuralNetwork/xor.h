#pragma once

#include <stdio.h>
#include <stdlib.h>


// predict output based on inputs
double *predict(char inputs[]);

// train neural network
void train(long epochs, double lr, char trainingInputs[4][2], char trainingOutputs[1][4]);

// save weights and biases to file
void save_xor(char *file);

// load weights and biases from file
void load_xor(char *file);

// init weights with random values
void init_weights();
