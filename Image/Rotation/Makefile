
# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 -g -fsanitize=address `pkg-config --cflags sdl2 SDL2_image` -D\_\_NO\_INLINE\_\_
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

all: rotate

SRC = rotate.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}


.PHONY: clean

clean:
	${RM} rotated.png
	${RM} ${OBJ}
	${RM} ${EXE}

# END
