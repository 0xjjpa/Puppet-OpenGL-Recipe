#ifndef IMAGE_H
#define IMAGE_H


#include <map>
#include <string>

#include "linalg.hpp"


// Class to represent PPM images loaded from disk. ALWAYS call
// Image.good() before using it.
class Image {
protected:

    int width_, height_, maxValue_;
    unsigned char *data_;

    // A mapping of filenames to already loaded images.
    static std::map<std::string,Image> cache_;

    // Read PPM data from a given file.
    bool readPPM(std::string const &filename);

public:

    // Public constructor which does nothing. In order for an Image to
    // be useful it must have image data via calling readPPM, but that
    // is hidden from public use. Ergo, construct Image instances with
    // the Image::fromCache static method.
    Image() : data_(NULL), width_(0), height_(0), maxValue_(0) {}

    // Retrieve an Image from the cache, or load it from disk if we have
    // not seen it before. Make sure to call Image.good() before using
    // it as this method will return a "bad" Image if the file was not
    // found or the file was corrupt.
    static Image& fromFile(std::string const &filename);

    // Tell us if the Image is ready to be used.
    bool good() const { return data_ != NULL; }

    // Look up the color (as an RGB vector) at the given UV coordinates (from 0 to 1).
    Vector lookup(double u, double v) const;

};


#endif
