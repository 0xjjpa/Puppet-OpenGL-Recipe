
#include <cstdio>
#include <cstdlib>
#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "linalg.hpp"
#include "mygl.hpp"
#include "scenario.hpp"


// Dimensions of the window we are drawing into.
static int realWidth = 400;
static int realHeight = 400;

// The size of our virtual pizels.
static int virtualPixelSize = 4;

// The scene that is displayed; starts with the most basic scene.
static int currentScene = 0;

// Flags to control some parts of the drawing pipeline. These are not static
// as they are also used in mygl.cpp.
bool gridIsVisible = false;
bool perspectiveCorrectTextures = true;
bool drawAsPoints = false;

// Which OBJ to display for scene H. Not static so it can be used in scenarios.cpp
std::string objFilename("teapot.obj");

// External camera attributes.
static Vector cameraPosition;
static Vector cameraFocus;
static bool usePerspective;

// The most recently seen mouse coordinates.
static int mouseX;
static int mouseY;
static int mouseModifiers;

// One-time configuration of many OpenGL directives.
static void initOpenGL(void) {

    // Set clearing values.
    glClearColor(0, 0, 0, 1);
    glClearDepth(1);

    // Enable and configure blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable and configure depth testing. We are using less-or-equal mode so
    // that points drawn later at the same depth override earlier ones.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize the point size to our virtual pixel size; we will draw
    // points as virtual pixels.
    glPointSize(virtualPixelSize);
}


// Called by GLUT at the start of the session, and when the window is reshaped.
static void reshape(int w, int h) {

    // Hold on to the new dimensions.
    realWidth = w;
    realHeight = h;

    // We want to draw into the whole window.
    glViewport(0, 0, w, h);

    // Calculate the virtual size of the window, and set it.
    double virtualWidth = realWidth / virtualPixelSize;
    double virtualHeight = realHeight / virtualPixelSize;
    myViewport(virtualWidth, virtualHeight);

    // Setup the projection matrix to map our virtual buffer into the real
    // window.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, virtualWidth - 0.5, -0.5, virtualHeight - 0.5, -1, 1);

    // Signal to GLUT that we need a redraw.
    glutPostRedisplay();
}


// Called by GLUT when you click the mouse.
// Simply saving mouse coords.
void mouseClick(int button, int state, int x, int y) {
    // GLUT Y coords are inverted.
    y = realHeight - y;
    mouseX = x;
    mouseY = y;
    mouseModifiers = glutGetModifiers();
}


// Called by GLUT when you move the mouse.
void mouseMove(int x, int y) {
    // GLUT Y coords are inverted.
    y = realHeight - y;

    // Calculate change from last known mouse positon.
    int dx = x - mouseX;
    int dy = y - mouseY;

    // Offset from the camera's focus to its position.
    Vector camOffset = cameraPosition - cameraFocus;

    // Unit vector to the camera from its focus.
    Vector camDir = camOffset.normalized();

    // Unit vector pointing to the right of the viewport.
    Vector right = Vector(0, 1, 0).cross(camDir).normalized();

    // Unit vector pointing to the top of the viewport.
    Vector up = camDir.cross(right).normalized();

    if (mouseModifiers & GLUT_ACTIVE_ALT) {
        // Dolly the camera into/out of the viewport.
        cameraPosition = cameraFocus + camOffset * (1 - 0.01 * dx);

    } else if (mouseModifiers & GLUT_ACTIVE_SHIFT) {
        // Pan the camera parallel to the viewport.
        cameraPosition = cameraPosition - right * dx * 0.025 - up * dy * 0.025;
        cameraFocus    = cameraFocus    - right * dx * 0.025 - up * dy * 0.025;

    } else {
        // Orbit the camera around the origin.
        cameraPosition = Matrix::rotation(0.5 * -3.14158 * dx / 180.0, Vector(0, 1, 0)) * cameraPosition;
        cameraPosition = Matrix::rotation(0.5 *  3.14159 * dy / 180.0, right) * cameraPosition;
    }

    // Remember mouse coords for next time.
    mouseX = x;
    mouseY = y;

    glutPostRedisplay();
}


// Called by GLUT when you press a key on the keyboard.
static void keyboard(unsigned char key, int x, int y) {

    switch(key) {

        // Quit.
        case 27: // escape
        case 'q':
            exit(0);
            break;

        // Toggle the grid.
        case '.':
            gridIsVisible = !gridIsVisible;
            break;

        // Disable grid, go to "real" pixel sizes.
        case '/':
            gridIsVisible = false;
            virtualPixelSize = 1;
            glPointSize(virtualPixelSize);
            reshape(realWidth, realHeight);
            break;

        // Decrease pixel size.
        case '<':
            if (virtualPixelSize > 1) {
                virtualPixelSize -= 1;
                glPointSize(virtualPixelSize);
                reshape(realWidth, realHeight);
            }
            break;

        // Increase pixel size.
        case '>':
            if (virtualPixelSize < 40) {
                virtualPixelSize += 2;
                glPointSize(virtualPixelSize);
                reshape(realWidth, realHeight);
            }
            break;

        // Toggle a couple flags.
        case 't':
            perspectiveCorrectTextures = !perspectiveCorrectTextures;
            break;
        case 'p':
            drawAsPoints = !drawAsPoints;
            break;
        case 'o':
            usePerspective = !usePerspective;
            break;

        // Pick a new scene.
        default:
            if (key - 'a' < scenarios.size()) {
                currentScene = key - 'a';
                scenarios[currentScene]->init(
                    cameraPosition,
                    cameraFocus,
                    usePerspective
                );
            }
            break;

    }

    // Signal GLUT that we need a redraw.
    glutPostRedisplay();
}



// Called by GLUT when we need to redraw the screen.
static void display(void) {

    // Prepare the myGL environment.
    myClear();
    myLoadIdentity();

    // Perspective projection (if on).
    if (usePerspective) {
        myFrustum(-0.5, 0.5, -0.5, 0.5, 1, 10);
    }

    // Setup camera to point at the cameraFocus from the cameraPosition.
    myLookAt(
        cameraPosition[0], cameraPosition[1], cameraPosition[2],
        cameraFocus[0], cameraFocus[1], cameraFocus[2],
        0, 2, 0
    );

    // Ask the scene to display itself.
    scenarios[currentScene]->display();

    // Transfer whatever we have drawn to the screen.
    glutSwapBuffers();
}


int main(int argc, char **argv) {

    // Initialize GLUT and open a window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(realWidth, realHeight);
    glutCreateWindow(argv[0]);

    // Register a bunch of callbacks for GLUT events.
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);

    // Setup OpenGL.
    initOpenGL();

    // Manually call reshape to assert virtual viewport is setup.
    reshape(realWidth, realHeight);

    // Setup the scenarios and pick the first one (by simulating a keystroke).
    initScenarios();
    keyboard('a', 0, 0);

    // Save the name of the OBJ file to display, and jump to that scene.
    if (argc > 1) {
        objFilename = argv[1];
        keyboard('h', 0, 0);
    }

    // Pass control to GLUT.
    glutMainLoop();
    return 0;
}

