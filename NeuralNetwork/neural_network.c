#include "xor.h"


// main function
int main(int argc, char *argv[])
{
	init_weights();

	// Training dataset
	char trainingInputs[4][2] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 }
	};
	char trainingOutputs[4][1] = { {0}, {1}, {1}, {0} };


	
	int Epochs = 15000;	
	double variance = 1.5f;
	
	printf("BEFORE TRAINING\n");
	for (size_t j = 0; j < 4; j++)
	{
		double o = predict(trainingInputs[j])[0];
		printf("For input [%hhi, %hhi] expected %hhi, predicted %f\n",
			trainingInputs[j][0], trainingInputs[j][1], trainingOutputs[j][0], o);
	}
	printf("\n");
	//train(Epochs, variance);
	load_xor("values");
	printf("AFTER TRAINING\n");
	for (size_t j = 0; j < 4; j++)
	{
		double o = predict(trainingInputs[j])[0];
		printf("For input [%hhi, %hhi] expected %hhi, predicted %f\n",
			trainingInputs[j][0], trainingInputs[j][1], trainingOutputs[j][0], o);
	}
	return 0;
}

