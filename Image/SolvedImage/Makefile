# Makefile

TARGET = Result
CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 -g -fsanitize=address `pkg-config --cflags sdl2 SDL2_image` -D\_\_NO\_INLINE\_\_
LDFLAGS = -fsanitize=address `pkg-config --cflags sdl2 SDL2_image` -lSDL2 -lSDL2_image -lSDL2_ttf
LDLIBS = -lm `pkg-config --cflags sdl2 SDL2_image`


SRC := $(shell find -name '*.c')
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: $(TARGET)

$(TARGET): ${OBJ}
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(TARGET)

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} ${TARGET}
	rm -f *.png

-include ${DEP}

# END
