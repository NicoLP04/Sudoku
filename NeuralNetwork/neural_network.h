#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>


// Number of Nodes
#define numInputs 784
#define numHiddens 120
#define numOutputs 10

// yes
SDL_Surface* load_image(const char* path);

// predict output based on inputs
int predict(SDL_Surface *image, char *file);

// train neural network
void train(long epochs, double lr, char *file, size_t numImages);

// save weights and biases to file
void save(char *file);

// load weights and biases from file
void load(char *file);

// init weights with random values
void init_weights();

void print_results();

void reset(char *file);
