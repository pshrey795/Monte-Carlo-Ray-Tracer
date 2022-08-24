CC=g++
CFLAGS=-Wall -g -O2 -std=c++17
INPUT_FILE=input.txt

rtrace: main.cpp
	$(CC) -o rtrace main.cpp $(CFLAGS)

run: rtrace 
	./rtrace $(INPUT_FILE) > Output/output.ppm

clean:
	rm -f rtrace 