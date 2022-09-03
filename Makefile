CC=g++
CFLAGS=-Wall -g -O3 -std=c++17 -lassimp
INPUT_FILE=cornell_box.obj
CONFIG_FILE=config.txt

rtrace: main.cpp src/Scene.cpp src/Mesh.cpp src/Camera.cpp src/vec3d.cpp
	$(CC) -o rtrace main.cpp src/Scene.cpp src/Mesh.cpp src/Camera.cpp src/vec3d.cpp $(CFLAGS)

run: rtrace 
	./rtrace Input/$(INPUT_FILE) Input/$(CONFIG_FILE)

clean:
	rm -f rtrace 