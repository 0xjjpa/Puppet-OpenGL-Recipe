#include <cstdio>
#include <iostream>
#include <fstream>

#include "image.hpp"


// Initialization of the cache map.
std::map<std::string,Image> Image::cache_;


bool Image::readPPM(std::string const &filename) {

    // Open and assert the file is good.
    std::ifstream file(filename.c_str());
    if (!file.good()) {
        std::cerr << "Unable to open PPM file \"" << filename << "\"" << std::endl;
        return false;
    }

    // Make sure this is a PPM file.
    unsigned char c1, c2;
    file >> c1;
    file >> c2;
    if (c1 != 'P' || c2 != '6') {
        std::cerr << "File \"" << filename << "\" is not a PPM file: " <<
            int(c1) << "," << int(c2) << std::endl;
        return false;
    }

    // Read image metadata.
    file >> width_;
    file >> height_;
    file >> maxValue_;

    // Discard a whitespace character.
    file.ignore(1);

    // Discard old data, if any.
    delete [] data_;

    // Read the data.
    int dataSize = width_ * height_ * 3;
    data_ = new unsigned char[dataSize];
    file.read((char*)data_, dataSize);

    // Were we successful? Clear out the data array if we weren't.
    bool success = !file.fail();
    if (!success) {
        delete [] data_;
    }

    // Clean up after ourselves.
    file.close();

    return success;
}


Image& Image::fromFile(std::string const &filename) {
    Image &image = Image::cache_[filename];
    if (!image.good()) {
        image.readPPM(filename);
    }
    return image;
}


Vector Image::lookup(double u, double v) const {

    // Clamp coords from 0 to 1.
    u = std::min(1.0, std::max(0.0, u));
    v = std::min(1.0, std::max(0.0, v));

    // Calculate pixel coords.
    int x = u * (width_ - 1);
    int y = v * (height_ - 1);

    // Calculate offset into data buffer.
    unsigned char *p = data_ + 3 * (y * width_ + x);

    // Convert to doubles from 0 to 1.
    return Vector(
        double(p[0]) / 255.0,
        double(p[1]) / 255.0,
        double(p[2]) / 255.0
    );

}
