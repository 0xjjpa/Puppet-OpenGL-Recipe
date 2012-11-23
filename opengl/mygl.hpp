#ifndef MYGL_H
#define MYGL_H


#include "linalg.hpp"


// Counterpart to glViewport; sets up the virtual window.
void myViewport(int width, int height);

// Counterpart to glClear; clears the real color buffer and the virtual
// depth buffer.
void myClear();

// Counterpart to glLoadIdentity; sets both projection and model-view matrix
// to be the identity.
void myLoadIdentity();

// Counterpart to glBindTexture; sets the current texture to that contained
// within the given file name. Pass NULL to turn off textures.
void myBindTexture(char const *name);

// EVERYTHING BELOW THIS LINE IS FOR YOU TO IMPLEMENT

// Counterpart to glBegin; tells the system what primitives we will be
// drawing.
void myBegin(int type);

// Counterpart to glColor3d; sets current color.
void myColor(double r, double g, double b);

// Counterpart to glVertex3d; store a vertex with the current color, texture
// coords, etc..
void myVertex(double x, double y, double z);

// Counterpart to glEnd; draws whatever primitives we have specified since the
// last myBegin().
void myEnd();

// Counterpart to glTranslated; multiplies the model-view matrix by the given
// translation.
void myTranslate(double tx, double ty, double tz);

// Counterpart to glRotated; multiplies the model-view matrix by the given
// rotation (in degrees) about the given axis.
void myRotate(double angle, double axisX, double axisY, double axisZ);

// Counterpart to glScaled; multiplies the model-view matrix by the given
// non-uniform scale.
void myScale(double sx, double sy, double sz);

// Counterpart to glFrustum; multiplies projection matrix with a
// transformation to use the given frustrum.
void myFrustum(double left, double right, double bottom, double top, double near, double far);

// Counterpart to gluLookAt; multiplies the model-view matrix to look from
// the given eye coords, to the given center coords, using the given up vector.
void myLookAt(double eyeX, double eyeY, double eyeZ,
              double cenX, double cenY, double cenZ,
              double  upX, double  upY, double  upZ);


// Counterpart to glTexCoord2d; sets current texture coordinates.
void myTexCoord(double s, double t);

// Counterpart to glNormal3d; sets current normals.
void myNormal(double x, double y, double z);


#endif
