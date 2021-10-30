# Compiler flags
CXX = g++
CXXFLAGS = -Wall

# Project files
SRC_DIR = ./src
LIB_DIR = ./dep
SRC_FILES = $(shell find $(SRC_DIR) -name *.cpp)
TARGET = app.exe


# Debug build settings
DEBUG_DIR = ./build/debug
DEBUG_EXE = $(DEBUG_DIR)/$(TARGET)
DEBUG_FLAGS = -g


# RELEASEease build settings
RELEASE_DIR = ./build/release
RELEASE_EXE = $(RELEASE_DIR)/$(TARGET)
RELEASE_FLAGS = -O3

.PHONY: all clean prep

# Default build
all: prep release

# Debug rules
debug: $(DEBUG_EXE)

$(DEBUG_EXE): $(SRC_FILES)
	$(CXX) -o $@ $(CXXFLAGS) $(DEBUG_FLAGS) $(SRC_FILES) -L$(LIB_DIR) -l:x64/xinput.lib

# Release rules
release: $(RELEASE_EXE)

$(RELEASE_EXE): $(SRC_FILES)
	$(CXX) -o $@ $(CXXFLAGS) $(RELEASE_FLAGS) $(SRC_FILES) -L$(LIB_DIR) -l:x64/xinput.lib


# Prepare build directory
prep:
	mkdir -p ./build
	mkdir -p $(DEBUG_DIR)
	mkdir -p $(RELEASE_DIR)

remake: clean all

clean:
	rm $(shell find -wholename *.exe)
