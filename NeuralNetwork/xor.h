#pragma once

#include <stdio.h>
#include <stdlib.h>


// Number of Nodes
#define numInputs 2
#define numHiddens 10
#define numOutputs 1


// predict output based on inputs
double *predict(char i1, char i2, char *file);

// train neural network
void train(long epochs, double lr, char trainingInputs[4][2],
    char trainingOutputs[4][1], char *file);

// save weights and biases to file
void save_xor(char *file);

// load weights and biases from file
void load_xor(char *file);

// init weights with random values
void init_weights();
