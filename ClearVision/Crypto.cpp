#include "Crypto.h"
#include "GrayscaleImage.h"
// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array;
    GrayscaleImage image = secret_image.reconstruct();
    int width = image.get_width();
    int height = image.get_height();
    int required_bits = message_length * 7;

    if (required_bits > width * height) {
        throw std::invalid_argument("not enough pixels to extract the message");
    }

    int message_index = 0;
    for (int i = height - 1; i >= 0; --i) {
        for (int j = width - 1; j >= 0; --j) {
            if (message_index < required_bits) {
                LSB_array.push_back(image.get_pixel(i, j) & 1);
                message_index++;
            }
        }
    }
    return LSB_array;
}
// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message;
    message = "";
    if (LSB_array.size() % 7 != 0) {
        throw std::invalid_argument("lsb array size not valid");
    }

    for (size_t i = 0; i < LSB_array.size(); i += 7) {
        std::bitset<7> bits;
        for (int j = 0; j < 7; ++j) {
            bits[j] = LSB_array[i + j] & 1;
        }
        message += char(bits.to_ulong());
    }
    return message;
}
// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array;
    for (char c : message) {
        std::bitset<7> bits(c);
        for (int i = 0; i < 7; ++i) {
            LSB_array.push_back(bits[i]);
        }
    }
    std::cout << LSB_array.size();
    return LSB_array;
}
// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {
    if (LSB_array.size() > image.get_width() * image.get_height()) { // pixel count = array size
        throw std::invalid_argument("Not enough space in the image to embed the message.");
    }
    
    int width = image.get_width();
    int height = image.get_height();
    int message_index = 0;

    for (int i = height - 1; i >= 0; --i) {
        for (int j = width - 1; j >= 0; --j) {
            if (message_index < LSB_array.size()) {
                int pixel = image.get_pixel(i, j);
                pixel &= ~1; // clear the least significant bit
                pixel |= LSB_array[message_index] & 1; // set the least significant bit
                image.set_pixel(i, j, pixel);
                message_index++;
            }
        }
    }
    SecretImage secret_image(image);

    return secret_image;
}
