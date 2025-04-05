#!/bin/bash

#!/bin/bash

# Compile the program
g++ -o legacy_shader main.cpp \
    -I/ucrt64/include/SDL2 \
    -L/ucrt64/lib \
    -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

sleep 0.5
# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Running the program..."
    ./legacy_shader
else
    echo "Compilation failed."
fi