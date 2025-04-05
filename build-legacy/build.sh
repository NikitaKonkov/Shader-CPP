#!/bin/bash

# Default texture path
TEXTURE_PATH="textures/grid.png"

# Check if texture path is provided as argument
if [ $# -ge 1 ]; then
    TEXTURE_PATH=$1
fi

# Compile the program
g++ -o legacy_shader src/main.cpp \
-I./include \
-lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32 -lSDL2_image

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Running the program with texture: $TEXTURE_PATH"
    ./legacy_shader "$TEXTURE_PATH"
else
    echo "Compilation failed."
fi