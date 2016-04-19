halftone: main.cpp
	c++ -std=c++11 main.cpp -O3 -lopencv_highgui -lopencv_core -o halftone

debug: main.cpp
	c++ -std=c++11 main.cpp -g -lopencv_highgui -lopencv_core -o halftone

clean:
	rm -f halftone
