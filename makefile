halftone: main.cpp
	c++ main.cpp -O3 -lopencv_highgui -lopencv_core -o halftone

debug: main.cpp
	c++ main.cpp -g -lopencv_highgui -lopencv_core -o halftone

clean:
	rm -f halftone
