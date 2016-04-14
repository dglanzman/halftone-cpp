all: main.cpp
	c++ main.cpp -O3 -lopencv_highgui -lopencv_core -o halftone
