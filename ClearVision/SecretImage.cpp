#include "SecretImage.h"

// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
    width = image.get_width();
    height = image.get_height();
    int size = width * height;
    int upper_size = (width * (width + 1)) / 2;
    int lower_size = (width * (width -1)) / 2;
    if (width != height) {
        throw std::invalid_argument("Image is not square");
    }
    upper_triangular = new int[upper_size];
    lower_triangular = new int[lower_size];

    int k = 0;
    int l = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (j >= i) {
                upper_triangular[k++] = image.get_pixel(i, j);
            }
            if (j < i) {
                lower_triangular[l++] = image.get_pixel(i, j);
            }
        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int w, int h, int * upper, int * lower) {
    width = w;
    height = h;
    upper_triangular = upper;
    lower_triangular = lower;
}


// Destructor: free the arrays
SecretImage::~SecretImage() {
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices.
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height);
    int k = 0;
    int l = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (j >= i) {
                image.set_pixel(i, j, upper_triangular[k++]);
            } else {
                image.set_pixel(i, j, lower_triangular[l++]);
            }
        }
    }
    return image;
}

// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {
    int k = 0;
    int l = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (j >= i) {
                upper_triangular[k++] = image.get_pixel(i, j);
            } else {
                lower_triangular[l++] = image.get_pixel(i, j);
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing.");
    }
    file << width << " " << height << std::endl; // ta said we should not use standard io redirection
    for (int i = 0; i < (width * (width + 1)) / 2; ++i) {
        file << upper_triangular[i] << (i == (width * (width + 1)) / 2 - 1 ? "" : " "); // no space after the last value
    }
    file << std::endl;
    for (int i = 0; i < (width * (width - 1)) / 2; ++i) {
        file << lower_triangular[i] << (i == (width * (width - 1)) / 2 - 1 ? "" : " ");
    }
    file << std::endl;
    file.close();
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading.");
    }
    int w, h;
    file >> w >> h;
    int upper_size = (w * (w + 1)) / 2;
    int lower_size = (w * (w - 1)) / 2;
    int *upper = new int[upper_size];
    int *lower = new int[lower_size];
    for (int i = 0; i < upper_size; ++i) {
        file >> upper[i];
    }
    for (int i = 0; i < lower_size; ++i) {
        file >> lower[i];
    }
    file.close();
    SecretImage secret_image(w, h, upper, lower);
    return secret_image;
}

// Returns a pointer to the upper triangular part of the secret image.
int * SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int * SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}
