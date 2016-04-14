halftone: main.cpp
	c++ main.cpp -O3 -lopencv_highgui -lopencv_core -o halftone

clean:
	rm -f halftone
