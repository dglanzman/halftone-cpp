#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** args) {
    
    // read input data
    if (argc < 2) {
        cout << "No filename given" << endl;
        return 0;
    }
    Mat input = imread(args[1]);
    if (input.data == NULL ||
            input.channels() != 3 ||
            input.depth() != CV_8U) {
        cout << "Invalid input data" << endl;
        return 0;
    }
    
    // make cyan
    for (int y = 0; y < input.rows; y++)
    for (int x = 0; x < input.cols; x++) {
        input.data[3 * (y * input.cols + x) + 1] = 255; // g channel
        input.data[3 * (y * input.cols + x)] = 255;     // b channel
    }

    // write out image
    imwrite("cyan.jpg", input);
}
