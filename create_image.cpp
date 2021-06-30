#include "lodepng.h"
#include <iostream>
#include <array>
#include <tuple>
#include <thread>
#include <math.h>
#include <stdlib.h>

const float A = 1.5;
const float B = .8;
const float C = 1.5;
const float D = 2.5;
const int num_points = 100000000;
const int iterations = 20;

const int scale = 200;
const int height = 1000;
const int width = 1000;

const float scaledMax = float(RAND_MAX) * 2. - 1.;
const int num_threads = 8;

inline std::tuple<float, float> new_point() {
    float x = rand() / scaledMax;
    float y = rand() / scaledMax;
    for (int i = 0; i < iterations; i++) {
        float temp_x = sin(A * y) + C * cos(A * x);
        float temp_y = sin(B * x) + D * cos(B * y);
        x = temp_x;
        y = temp_y;
    }
    return std::make_tuple(x, y);
}

inline int cap(int num, int min, int max) {
    return std::min(std::max(num, min), max);
}

inline std::array<int, 3> color(int temperature) {
    // Converts a temperature to a fiery color in RGB,
    // representing one pixel in the heatmap
    const int red = cap(int(temperature), 0, 255);
    const int green = cap(int(temperature) - 255, 0, 255);
    const int blue = cap(int(temperature) - 255 * 2, 0, 255);
    return {red, green, blue};
}

void create_image(int heatmap[height][width], int width, int height) {
    u_char* image_data = new u_char[3 * height * width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int img_index = 3 * (i * width + j);
            std::array<int, 3> rgb = color(heatmap[i][j]);
            image_data[img_index]   = rgb[0];
            image_data[img_index+1] = rgb[1];
            image_data[img_index+2] = rgb[2];
        }
    }
    lodepng::encode("image.png", image_data, width, height, LCT_RGB);
}

std::vector<std::tuple<float, float>> generate_points(int n, int heatmap[height][width]) {
    // returns a list of n points
    std::vector<std::tuple<float, float>> points = {};
	float halfHeight = height / 2.;
	float halfWidth = width / 2.;
    for (int point = 0; point < n; point++) {
        float x, y;
        std::tie(x, y) = new_point();
        int i = halfHeight - scale * y;
        int j = halfWidth - scale * x;
        if (i >= 0 && i < height && j >= 0 && j < width) {
            heatmap[i][j]++;
        }
    }
    return points;
}


int main() {
    int heatmap[height][width];
    std::cout << "Drawing " << num_points << " points using " << num_threads << " threads..." << std::endl;
    std::array<std::thread, num_threads> threads;
    std::vector<std::tuple<float, float>> points = {};
    for (int i = 0; i < num_threads; i++) {
        threads[i] = std::thread(generate_points, num_points / num_threads, heatmap);
    }
    for (std::thread& current_thread: threads) {
        current_thread.join();
    }
    create_image(heatmap, width, height);
}
