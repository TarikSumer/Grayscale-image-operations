#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>
#include <iostream>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {

    int height = image.get_height();
    int width = image.get_width();
    GrayscaleImage result(width, height);
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0;

    // creating kernel
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            double x = i - (kernelSize - 1) / 2.0; // (i - kernelSize) may behave unexpectedly because of integer division
            double y = j - (kernelSize - 1) / 2.0;
            kernel[i][j] = (1.0 / (2.0 * M_PI * sigma * sigma)) * exp(-((x * x) + (y * y)) / (2.0 * sigma * sigma));
            sum += kernel[i][j];
        }
    }

    // normalization
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            kernel[i][j] /= sum;
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double weightedSum = 0;
            for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x) {
                for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y) {
                    int row = i + x;
                    int col = j + y;
                    if (row >= 0 && row < height && col >= 0 && col < width) { // only in bound pixels will be added
                        weightedSum += image.get_pixel(row, col) * kernel[x + kernelSize / 2][y + kernelSize / 2];
                    }
                }
            }
            result.set_pixel(i, j, std::max(0, std::min(255, static_cast<int>(weightedSum)))); // modified but not tested yet

        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image.set_pixel(i, j, result.get_pixel(i, j));
        }
    }
}

// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {
    // no need for checking for valid kernel size
    int height = image.get_height();
    int width = image.get_width();
    GrayscaleImage result(width, height); // dynamic allocation

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double sum = 0;
            int count = 0;
            for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x) {
                for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y) {
                    int row = i + x;
                    int col = j + y;
                    if (row < height && row >= 0 && col < width && col >= 0) {
                        sum += image.get_pixel(row, col);
                        count++;
                    } else {
                        count++;
                    }
                }
            }
            result.set_pixel(i, j, static_cast<int>(sum / count));
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image.set_pixel(i, j, result.get_pixel(i, j));
        }
    }
}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {
    GrayscaleImage blurred(image);
    apply_gaussian_smoothing(blurred, kernelSize, 1.0);
    int width = image.get_width();
    int height = image.get_height();

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            int original = image.get_pixel(i, j);
            int edge = original - blurred.get_pixel(i, j);
            int sharpened = original + static_cast<int>(amount * edge);
            image.set_pixel(i, j, std::min(255, std::max(0, sharpened)));
        }
    }
}
