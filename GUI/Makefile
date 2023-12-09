# Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -O3 -g
LDLIBS = `pkg-config --libs gtk+-3.0`

EXE = main interface

all: $(EXE)
	mv interface ../

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
