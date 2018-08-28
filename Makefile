OBJS = main.c scene_base.c scene_mainmenu.c
CC = gcc
COMPILER_FLAGS = -w
LINKER_FLAGS = -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_image -lSDL2_ttf
OBJ_NAME = game

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
