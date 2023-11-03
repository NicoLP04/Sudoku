#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define KNRM  "\x1B[0m"
//#define KRED  "\x1B[31m"

#define size 9
char s[size][size] = {};


int is_valid(size_t row, size_t col, char val)
{
	// verify if row is valid
	for (size_t i = 0; i < size; i++)
		if (s[i][col] == val)
			return 0;

	//verify is col is valid
	for (size_t i = 0; i < size; i++)
		if (s[row][i] == val)
			return 0;

	//verify is box is valid
	size_t a = row / 3 * 3;
	size_t b = col / 3 * 3;
	size_t c = row / 3 * 3 + 3;
	size_t d = col / 3 * 3 + 3;

	for (size_t i = a; i < c; i++)
		for (size_t j = b; j < d; j++)
			if (s[i][j] == val)
				return 0;

	return 1;

}


int solve(size_t row, size_t col)
{
	if (row == 9)
		return 1;
	else if (col == 9)
		return solve(row+1, 0);
	else if (s[row][col] != 0)
		return solve(row, col+1);
	else
	{
		for (size_t k = 1; k < 10; k++)
		{
			if (is_valid(row, col, k) == 1)
			{
				s[row][col] = k;
				if (solve(row, col+1))
					return 1;
				s[row][col] = 0;
			}
		}
		return 0;
	}
}


void print_grid()
{
	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			printf("%hhi", s[i][j]);
			if ((j+1) % 3 == 0 && j != 8)
				printf(" ");
		}
		printf("\n");
		if ((i+1) % 3 == 0 && i != 8)
			printf("\n");
	}
}




void read_grid(char* fileName)
{
	FILE* file = fopen(fileName, "r");

	if (!file)
	{
		printf("\n Unable to open : %s ", fileName);
		exit(EXIT_FAILURE);
}

	char line[500];

	size_t i = 0;
	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '\n')
			continue;
		size_t j = 0;
		size_t z = 0;
		while (line[z] != 0)
		{
			if (line[z] >= '0' && line[z] <= '9')
			{
				s[i][j] = line[z] - '0';
				j++;
			}
			else if (line[z] == '.')
			{
				s[i][j] = 0;
				j++;
			}
			z++;
		}
		i++;
	}

	fclose(file);
}


void write_grid(char* fileName)
{
	strcat(fileName, ".result");
        FILE* file = fopen(fileName, "w");

        for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			fputc(s[i][j] + '0', file);
			if ((j+1) % 3 == 0 && j != 8)
				fputc(' ', file);
		}
		fputc('\n', file);
		if ((i+1) % 3 == 0 && i != 8)
			fputc('\n', file);
        }

	fclose(file);
}


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Error: expected 1 argument, got: %i", argc - 1);
		return -1;
	}

	//read any text file from currect directory
	char* fileName = argv[1];
	read_grid(fileName);

	solve(0, 0);
	write_grid(fileName);

	return 0;
}
