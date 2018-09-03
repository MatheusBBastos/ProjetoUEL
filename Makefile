OBJS = main.c scene_base.c scene_mainmenu.c scene_singleplayer.c widgets.c network.c
CC = gcc
COMPILER_FLAGS = -w
LINKER_FLAGS = -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
OBJ_NAME = game

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) 

get-deps:
	sudo apt-get update;
	sudo apt-get update --fix-missing;
	sudo apt-get install libegl1-mesa-dev;
	sudo apt-get install libgles2-mesa-dev;
	sudo apt-get install libsdl2-dev;
	sudo apt-get install libsdl2-image-dev;
	sudo apt-get install libsdl2-ttf-dev;
	sudo apt-get install libsdl2-mixer-dev;