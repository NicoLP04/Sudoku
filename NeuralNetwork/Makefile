# Makefile

TARGET = NeuralNetwork
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 -g -fsanitize=address
LDFLAGS = -fsanitize=address
LDLIBS = -lm

SRC = xor.c maths_func.c neural_network.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: $(TARGET)

$(TARGET): ${OBJ}
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(TARGET)

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} $(TARGET)

-include ${DEP}

# END
