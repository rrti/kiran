CC = g++
CFLAGS = -Wall -Wextra -g -Wno-strict-aliasing -O2 -ffast-math -fomit-frame-pointer
LFLAGS = -lSDL -lboost_thread -llua5.1

MKDIR = mkdir
TARGET = kiran

SRC = ./src
OBJ = ./obj
BIN = ./bin

BASE_SRC_DIR        = $(SRC)
BASE_OBJ_DIR        = $(OBJ)
BASE_BIN_DIR        = $(BIN)
DATASTRUCTS_SRC_DIR = $(BASE_SRC_DIR)/datastructs
DATASTRUCTS_OBJ_DIR = $(BASE_OBJ_DIR)/datastructs
MATH_SRC_DIR        = $(BASE_SRC_DIR)/math
MATH_OBJ_DIR        = $(BASE_OBJ_DIR)/math
RENDERER_SRC_DIR    = $(BASE_SRC_DIR)/renderer
RENDERER_OBJ_DIR    = $(BASE_OBJ_DIR)/renderer
SYSTEM_SRC_DIR      = $(BASE_SRC_DIR)/system
SYSTEM_OBJ_DIR      = $(BASE_OBJ_DIR)/system

MATH_OBS = \
	$(MATH_OBJ_DIR)/vec3.o
DATASTRUCTS_OBJS = \
	$(DATASTRUCTS_OBJ_DIR)/KDTree.o \
	$(DATASTRUCTS_OBJ_DIR)/PhotonMap.o \
	$(DATASTRUCTS_OBJ_DIR)/UniformGrid.o
RENDERER_OBS = \
	$(RENDERER_OBJ_DIR)/Camera.o \
	$(RENDERER_OBJ_DIR)/RayTracer.o \
	$(RENDERER_OBJ_DIR)/Scene.o \
	$(RENDERER_OBJ_DIR)/SceneObject.o \
	$(RENDERER_OBJ_DIR)/MaterialReflectionModel.o
SYSTEM_OBS = \
	$(SYSTEM_OBJ_DIR)/LuaParser.o \
	$(SYSTEM_OBJ_DIR)/RNG.o \
	$(SYSTEM_OBJ_DIR)/Main.o \
	$(SYSTEM_OBJ_DIR)/Profiler.o \
	$(SYSTEM_OBJ_DIR)/SDLWindow.o

OBJECTS = \
	$(DATASTRUCTS_OBJS) \
	$(MATH_OBS) \
	$(RENDERER_OBS) \
	$(SYSTEM_OBS)



$(DATASTRUCTS_OBJ_DIR)/%.o: $(DATASTRUCTS_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(MATH_OBJ_DIR)/%.o: $(MATH_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(RENDERER_OBJ_DIR)/%.o: $(RENDERER_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(SYSTEM_OBJ_DIR)/%.o: $(SYSTEM_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<


default:
	make dirs
	make objects
	make target

dirs:
	if [ ! -d $(BASE_BIN_DIR) ];        then $(MKDIR) $(BASE_BIN_DIR); fi
	if [ ! -d $(BASE_OBJ_DIR) ];        then $(MKDIR) $(BASE_OBJ_DIR); fi
	if [ ! -d $(DATASTRUCTS_OBJ_DIR) ]; then $(MKDIR) $(DATASTRUCTS_OBJ_DIR); fi
	if [ ! -d $(MATH_OBJ_DIR) ];        then $(MKDIR) $(MATH_OBJ_DIR); fi
	if [ ! -d $(RENDERER_OBJ_DIR) ];    then $(MKDIR) $(RENDERER_OBJ_DIR); fi
	if [ ! -d $(SYSTEM_OBJ_DIR) ];      then $(MKDIR) $(SYSTEM_OBJ_DIR); fi

objects: $(OBJECTS)

target:
	$(CC) $(LFLAGS) -o $(BIN)/$(TARGET)    $(OBJECTS)

clean:
	rm -rf $(BIN)/
	rm -rf $(OBJ)/
