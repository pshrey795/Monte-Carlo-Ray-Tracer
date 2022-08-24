CC=g++
CFLAGS=-Wall -g -O2 -std=c++17 -lassimp
INPUT_FILE=input.txt
CONFIG_FILE=config.txt

rtrace: main.cpp
	$(CC) -o rtrace main.cpp src/Scene.cpp src/Mesh.cpp src/Camera.cpp src/vec3d.cpp $(CFLAGS)

run: rtrace 
	./rtrace $(INPUT_FILE) $(CONFIG_FILE) > Outputs/output.ppm

clean:
	rm -f rtrace 