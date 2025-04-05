rm -f shadertoy_renderer.exe

sleep 0.5

g++ -o shadertoy_renderer main.cpp shader_manager.cpp shadertoy_utils.cpp -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32

sleep 1

./shadertoy_renderer.exe