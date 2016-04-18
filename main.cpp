#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void halftone_cir(Mat img, int size) {
    // for each block corresponding to one halftone
    for (int by = 0; by < img.rows / size; by++)
    for (int bx = 0; bx < img.cols / size; bx++) {
        // find the mean brightness
        double mean = 0;
        int x, y, sx, sy;
        for (y = by * size, sy = y; y - sy < size; y++)
        for (x = bx * size, sx = x; x - sx < size; x++) {
            mean += img.data[y * img.cols + x];
        }
        mean /= (size * size);

        // convert brightness to squared radius of halftone
        double r_sq = (double)((255 - mean) * size * size) / 510;

        // write the halftone cell
        for (y = by * size, sy = y; y - sy < size; y++)
        for (x = bx * size, sx = x; x - sx < size; x++) {
            char color;
            int mid_y = sy + size/2;
            int mid_x = sx + size/2;
            int dis_sq = (x-mid_x) * (x-mid_x) + (y-mid_y) * (y-mid_y);
            if (dis_sq < r_sq) {
                color = 0;
            } else {
                color = 255;
            }
            img.data[y * img.cols + x] = color;
        }
    }
}

void halftone_sq(Mat img, int size) {

    // check inputs
    if (size > 256) {
        cout << "only supports maximum halftone size of 256" << endl;
        return;
    }

    // for each block corresponding to one halftone
    for (int by = 0; by < img.rows / size; by++)
    for (int bx = 0; bx < img.cols / size; bx++) {
        // find the mean brightness
        double mean = 0;
        int x, y, sx, sy;
        for (y = by * size, sy = y; y - sy < size; y++)
        for (x = bx * size, sx = x; x - sx < size; x++) {
            mean += img.data[y * img.cols + x];
        }
        mean /= (size * size);

        // convert brightness to width of the halftone
        int width = round(size * sqrt((255 - mean)/255));

        // write the background white
        for (y = by * size, sy = y; y - sy < size; y++)
        for (x = bx * size, sx = x; x - sx < size; x++) {
            img.data[y * img.cols + x] = 255;
        }

        // write the halftone black
        for (y = by * size + (size - width) / 2, sy = y; y - sy < width; y++)
        for (x = bx * size + (size - width) / 2, sx = x; x - sx < width; x++) {
            img.data[y * img.cols + x] = 0;
        }
    }
}

Mat gray(Mat input) {
    Mat output(input.rows, input.cols, CV_8UC1);
    for (int y = 0; y < output.rows; y++)
    for (int x = 0; x < output.cols; x++) {
        int mean = 0;
        mean += input.data[3 * (y * input.cols + x)];
        mean += input.data[3 * (y * input.cols + x) + 1];
        mean += input.data[3 * (y * input.cols + x) + 2];
        mean /= 3;
        output.data[y * output.cols + x] = mean;
    }
    return output;
}

Mat rotate(Mat input, double angle) {
    // make output buffer
    int outsize = input.rows > input.cols ? input.rows : input.cols;
    outsize = ceil(outsize * sqrt(2));
    Mat output(outsize, outsize, CV_8UC3);

    // for a given output pixel...
    for (int y = 0; y < output.rows; y++)
    for (int x = 0; x < output.cols; x++) {
        
        // ...use a rotation matrix to get the corresponding input pixel
        int cx = x - output.cols / 2;
        int cy = y - output.rows / 2;
        double cs = cos(angle);
        double sn = sin(angle);
        int rx = round(cx * cs + cy * -sn);
        int ry = round(cx * sn + cy * cs);
        rx += input.cols / 2;
        ry += input.rows / 2;

        // if the input pixel coordinates are within the original image,
        // save that pixel data
        char r, g, b;
        if (rx > 0 && ry > 0 && rx < input.cols && ry < input.rows) {
            b = input.data[3 * (ry * input.cols + rx)];
            g = input.data[3 * (ry * input.cols + rx) + 1];
            r = input.data[3 * (ry * input.cols + rx) + 2];
        } else { // otherwise, use white
            b = 255;
            g = 255;
            r = 255;
        }

        // write the pixel data to the output pixel location
        output.data[3 * (y * output.cols + x)] = b;
        output.data[3 * (y * output.cols + x) + 1] = g;
        output.data[3 * (y * output.cols + x) + 2] = r;
    }

    return output;
}

Mat unrotate(Mat input, double angle, int original_y, int original_x) {
    Mat output(original_y, original_x, CV_8UC1);
    for (int y = 0; y < output.rows; y++)
    for (int x = 0; x < output.cols; x++) {
        int cx = x - output.cols / 2;
        int cy = y - output.rows / 2;
        double cs = cos(angle);
        double sn = sin(angle);
        int rx = round(cx * cs + cy * -sn);
        int ry = round(cx * sn + cy * cs);
        rx += input.cols / 2;
        ry += input.rows / 2;
        output.data[y * output.cols + x] = input.data[ry * input.cols + rx];
    }
    return output;
}

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
    
    double angle = 0.261799; // is 15 deg
    //double angle = 3.14159265/4; // is 45 deg
    //double angle = 0; // is 0 deg
    Mat prep = gray(rotate(input, angle));
    halftone_cir(prep, 24);
    Mat output = unrotate(prep, -angle, input.rows, input.cols);

    // write out image
    imwrite("after.jpg", output);
}
