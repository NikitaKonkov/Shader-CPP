#!/bin/bash

# Compile the program
g++ -o legacy_shader src/main.cpp \
    -I./include \
    -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Running the program..."
    ./legacy_shader
else
    echo "Compilation failed."
fi