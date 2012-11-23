/////////////////////////////////////////////////////////////
// FILE:      mygl.cpp
// CONTAINS:  your implementations of various GL functions
////////////////////////////////////////////////////////////.


#include <cstdlib>
#include <cfloat>
#include <iostream>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "image.hpp"
#include "mygl.hpp"


// Flags that are defined/set in a3.cpp.
extern bool perspectiveCorrectTextures;
extern bool gridIsVisible;
extern bool drawAsPoints;

// The dimensions of the virtual window we are drawing into.
int virtualWidth;
int virtualHeight;

// The current projection and model-view matrices.
Matrix projectionMatrix;
Matrix modelViewMatrix;

// Current color, texture, and texture coordinates.
Vector currentColor;
Image *currentTexture;
double currentTextureCoord[2];
Vector currentNormal;


// A class to simplify lookup of two-dimensional zBuffer data from an array
// of doubles. This zBuffer MUST have reshape(...) called before use.
class ZBuffer {
private:

    int height_, size_, allocated_;
    double *data_;

public:

    ZBuffer() : allocated_(0), data_(NULL) {}

    // Reshape the zBuffer because the window was reshaped.
    void reshape(int w, int h) {
        height_ = h;
        size_ = w * h;
        // Only need to bother reallocating the array when we need a larger
        // array.
        if (size_ > allocated_) {
            delete [] data_;
            allocated_ = 4 * size_;
            data_ = new double[allocated_];
        }
    }

    // Clear out all of the depth values.
    void clear() {
        for (int i = 0; i < size_; i++) {
            data_[i] = DBL_MAX;
        }
    }

    // Get a pointer to the given row of depth data, which can be indexed
    // again to get/set depth values.
    // E.g.: `zBuffer[x][y] = newDepthValue`.
    double* operator[](int index) {
        return data_ + index * height_;
    }

} zBuffer;


// A function to set a pixel value on the screen. This is the entry point that
// you MUST use to draw to the screen.
void setPixel(int x, int y, double r, double g, double b)
{
    if (x < 0 || x >= virtualWidth || y < 0 || y >= virtualHeight) {
        std::cerr << "attempting to set a pixel that is off-screen;" << x << ", " << y << std::endl;
        return;
    }
    glColor3d(r, g, b);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}


// Draws the virtual pixel grid.
void drawPixelGrid() {

    // Dark gray.
    glColor4d(0.15, 0.15, 0.15, 1.0);

    // Draw vertical grid lines.
    for (float x = -0.5; x <= virtualWidth; x++) {
        glBegin(GL_LINES);
        glVertex3f(x, -0.5, 1.0);
        glVertex3f(x, virtualHeight + 0.5, 1.0);
        glEnd();
    }

    // Draw horizontal grid lines.
    for (float y = -0.5; y <= virtualHeight; y++) {
        glBegin(GL_LINES);
        glVertex3f(-0.5, y, 1.0);
        glVertex3f(virtualWidth + 0.5, y, 1.0);
        glEnd();
    }
}


void myViewport(int w, int h) {
    virtualWidth = w;
    virtualHeight = h;
    zBuffer.reshape(w, h);
}


void myClear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (gridIsVisible) {
        drawPixelGrid();
    }
    zBuffer.clear();
}


void myLoadIdentity() {
    modelViewMatrix = Matrix::identity();
    projectionMatrix = Matrix::identity();
}


void myBindTexture(char const *name) {
    if (name) {
        currentTexture = &Image::fromFile(name);
    } else {
        currentTexture = NULL;
    }
}


void myBegin(int type) {
    // @@@@ YOUR CODE HERE
}


void myColor(double r, double g, double b) {
    // @@@@ YOUR CODE HERE
}


void myVertex(double x, double y, double z) {
    // @@@@ YOUR CODE HERE
}


void myEnd() {
    // @@@@ YOUR CODE HERE
}


void myTranslate(double tx, double ty, double tz) {
    // @@@@ YOUR CODE HERE
}


void myRotate(double angle, double axisX, double axisY, double axisZ) {
    // @@@@ YOUR CODE HERE
    // @@@@ NOTE: Make sure you're aware of degrees vs. radians!
}


void myScale(double sx, double sy, double sz) {
    // @@@@ YOUR CODE HERE
}


void myFrustum(double left, double right, double bottom, double top, double near, double far) {
    // @@@@ YOUR CODE HERE
}


void myLookAt(double eyeX, double eyeY, double eyeZ,
              double cenX, double cenY, double cenZ,
              double  upX, double  upY, double  upZ) {
    // @@@@ YOUR CODE HERE
}


void myTexCoord(double s, double t) {
    // @@@@ YOUR CODE HERE
}


void myNormal(double x, double y, double z) {
    // @@@@ YOUR CODE HERE
}

