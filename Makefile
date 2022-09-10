CC=g++
CFLAGS=-Wall -g -O3 -std=c++17 -lassimp -fopenmp
INPUT_FILE=cornell_box.obj
CONFIG_FILE=config.txt

rtrace: main.cpp src/*.cpp
	$(CC) -o rtrace main.cpp src/*.cpp $(CFLAGS)

run: rtrace 
	./rtrace Input/$(INPUT_FILE) Input/$(CONFIG_FILE) > Outputs/output.ppm

show: Outputs/output.ppm show.sh
	chmod a+x show.sh
	./show.sh

clean:
	rm -f rtrace 