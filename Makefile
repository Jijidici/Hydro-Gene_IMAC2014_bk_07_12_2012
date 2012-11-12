CC = g++
CFLAGS = -Wall -ansi -pedantic -I include -O2
LDFLAGS = -lSDL -lGL -fopenMP

SRC_VOXEL_PATH = src_make_voxel
SRC_DISPLAY_PATH = src_display
BIN_PATH = bin

EXEC_VOXEL = hg_voxel_maker
EXEC_DISPLAY = hg_display

SRC_VOXEL_FILES = $(shell find $(SRC_VOXEL_PATH) -type f -name '*.cpp')
OBJ_VOXEL_FILES = $(patsubst $(SRC_VOXEL_PATH)/%.cpp, $(SRC_VOXEL_PATH)/%.o, $(SRC_VOXEL_FILES))

SRC_DISPLAY_FILES = $(shell find $(SRC_DISPLAY_PATH) -type f -name '*.cpp')
OBJ_DISPLAY_FILES = $(patsubst $(SRC_DISPLAY_PATH)/%.cpp, $(SRC_DISPLAY_PATH)/%.o, $(SRC_DISPLAY_FILES))

all: $(BIN_PATH)/$(EXEC_VOXEL) $(BIN_PATH)/$(EXEC_DISPLAY)

$(BIN_PATH)/$(EXEC_VOXEL): $(OBJ_VOXEL_FILES) $(SRC_VOXEL_PATH)/glew-1.9/glew.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_PATH)/$(EXEC_DISPLAY): $(OBJ_DISPLAY_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRC_VOXEL_PATH)/glew-1.9/glew.o: $(SRC_VOXEL_PATH)/glew-1.9/glew.c
	$(CC) -c -o $@ $(CFLAGS) $^ 

$(SRC_VOXEL_PATH)/%.o: $(SRC_VOXEL_PATH)/%.cpp
	$(CC) -c -o $@ $(CFLAGS) $^ 

$(SRC_DISPLAY_PATH)/%.o: $(SRC_DISPLAY_PATH)/%.cpp
	$(CC) -c -o $@ $(CFLAGS) $^ 

clean:
	rm $(OBJ_VOXEL_FILES) $(OBJ_DISPLAY_FILES) $(SRC_VOXEL_PATH)/glew-1.9/glew.o

cleanall:
	rm $(BIN_PATH)/$(EXEC_VOXEL) $(BIN_PATH)/$(EXEC_DISPLAY) $(OBJ_VOXEL_FILES) $(OBJ_DISPLAY_FILES) $(SRC_VOXEL_PATH)/glew-1.9/glew.o
