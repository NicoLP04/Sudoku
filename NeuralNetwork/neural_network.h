#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>


// Number of Nodes
#define numInputs 784
#define numHiddens 150
#define numOutputs 9


// predict output based on inputs
double *predict(char *file, SDL_Surface *image);

// train neural network
void train(long epochs, double lr, char *file, size_t numImages, size_t batchSize);

// save weights and biases to file
void save(char *file);

// load weights and biases from file
void load(char *file);

// init weights with random values
void init_weights();

void print_results(char *file);

void reset(char *file);
