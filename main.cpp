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
        char c;
        if (rx > 0 && ry > 0 && rx < input.cols && ry < input.rows) {
            c = input.data[ry * input.cols + rx];
        } else { // otherwise, use white
            c = 255;
        }

        // write the pixel data to the output pixel location
        output.data[y * output.cols + x] = c;
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

vector<Mat> split(Mat rgb) {
    Mat r(rgb.rows, rgb.cols, CV_8UC1);
    Mat g(rgb.rows, rgb.cols, CV_8UC1);
    Mat b(rgb.rows, rgb.cols, CV_8UC1);
    for (int y = 0; y < rgb.rows; y++)
    for (int x = 0; x < rgb.cols; x++) {
        r.data[y * r.cols + x] = rgb.data[3 * (y * rgb.cols + x) + 2];
        g.data[y * r.cols + x] = rgb.data[3 * (y * rgb.cols + x) + 1];
        b.data[y * r.cols + x] = rgb.data[3 * (y * rgb.cols + x) + 0];
    }
    vector<Mat> v;
    v.push_back(r);
    v.push_back(g);
    v.push_back(b);
    return v;
}

Mat merge(vector<Mat> rgb) {
    Mat out(rgb[0].rows, rgb[0].cols, CV_8UC3);
    for (int y = 0; y < out.rows; y++)
    for (int x = 0; x < out.cols; x++) {
        out.data[3 * (y * out.cols + x) + 0] = rgb[2].data[y * out.cols + x];
        out.data[3 * (y * out.cols + x) + 1] = rgb[1].data[y * out.cols + x];
        out.data[3 * (y * out.cols + x) + 2] = rgb[0].data[y * out.cols + x];
    }
    return out;
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
    
    vector<Mat> rgb = split(input);
    vector<double> angles = {0.261799, 1.309, 0.785398};
    vector<Mat> restored;

    for (int i = 0; i < 3; i++) {
        Mat rotated = rotate(rgb[i], angles[i]);
        halftone_cir(rotated, 4);
        restored.push_back(unrotate(rotated, -angles[i], rgb[i].rows, rgb[i].cols));
    }

    Mat output = merge(restored);

    // write out image
    imwrite("after.jpg", output);
}
