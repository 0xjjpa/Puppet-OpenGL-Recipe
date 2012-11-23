#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "mygl.hpp"
#include "linalg.hpp"
#include "object.hpp"
#include "scenario.hpp"


// The name of the OBJ file as defined by a3.cpp
extern std::string objFilename;


// The scenario list.
std::vector<Scenario*> scenarios;


// Default camera settings
void Scenario::init(Vector &cam, Vector &focus, bool &persp) const {
    cam = Vector(0, 0, 2);
    focus = Vector(0, 0, 0);
    persp = false;
}


// Tests a couple of points.
class ScenarioA : public Scenario {
    void display() const {

        myBegin(GL_LINES);

        // Blue from centre to right.
        myColor(0, 0, 1);
        myVertex(0, 0, 0);
        myVertex(0.5, 0, 0);

        // Green from right to top.
        myColor(0, 1, 0);
        myVertex(0.5, 0, 0);
        myVertex(0.25, 0.5, 0);

        // Red from top to centre.
        myColor(1, 0, 0);
        myVertex(0.25, 0.5, 0);
        myVertex(0, 0, 0);

        myEnd();
    }
};


// Tests basic transformations.
class ScenarioB : public Scenario {
    void display() const {

        myTranslate(0, -1, 0);
        myRotate(90, 0, 0, 1);
        myScale(2, 2, 2);

        myBegin(GL_TRIANGLES);
        myColor(1, 1, 1);
        myVertex(0, 0, 0);
        myVertex(0.5, 0, 0);
        myVertex(0.5, 0.5, 0);
        myEnd();
    }
};


// Tests viewing and projection transformations
class ScenarioC : public Scenario {

    // Specific camera for this scene.
    void init(Vector &cam, Vector &focus, bool &persp) const {
        cam = Vector(1.65, 1.25, 1.18);
        focus = Vector(0, 0, 0);
        persp = true;
    }

    void display() const {

        myTranslate(-0.5, -0.5, -0.5);

        // X+ face of cube
        myColor(1, 0, 0);
        myBegin(GL_TRIANGLES);
        myVertex(1, 1, 0);
        myVertex(1, 0, 0);
        myVertex(1, 1, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myVertex(1, 0, 0);
        myVertex(1, 1, 1);
        myVertex(1, 0, 1);
        myEnd();

        // Y+ face of cube
        myColor(0, 1, 0);
        myBegin(GL_TRIANGLES);
        myVertex(1, 1, 0);
        myVertex(0, 1, 0);
        myVertex(1, 1, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myVertex(0, 1, 0);
        myVertex(1, 1, 1);
        myVertex(0, 1, 1);
        myEnd();

        // Z+ face of cube
        myColor(0, 0, 1);
        myBegin(GL_TRIANGLES);
        myVertex(0, 1, 1);
        myVertex(1, 1, 1);
        myVertex(0, 0, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myVertex(1, 1, 1);
        myVertex(0, 0, 1);
        myVertex(1, 0, 1);
        myEnd();
    }
};


// Tests a couple basic triangles.
class ScenarioD : public Scenario {
    void display() const {

        // Red triangle with vertices at the same location as the points in A.
        myColor(1, 0, 0);
        myBegin(GL_TRIANGLES);
        myVertex(0, 0, 0);
        myVertex(0.5, 0, 0);
        myVertex(0.5, 0.5, 0);
        myEnd();

        // Blue triangle half off-screen to test clipping.
        myColor(0, 0, 1);
        myBegin(GL_TRIANGLES);
        myVertex(-0.5, 0.5, 0);
        myVertex(-0.5, -0.5, 0);
        myVertex(-1.5, 0, 0);
        myEnd();
    }
};


// Tests color interpolation.
class ScenarioE : public Scenario {
    void display() const {

        // A line with two different colors.
        myBegin(GL_LINES);
        myColor(0, 1, 0);
        myVertex(-0.25, 0.5, 0);
        myColor(1, 0, 0);
        myVertex(-0.5, -0.25, 0);
        myEnd();

        // A triangle with the same colors/positions as in A.
        myBegin(GL_TRIANGLES);
        myColor(0, 0, 1);
        myVertex(0, 0, 0);
        myColor(0, 1, 0);
        myVertex(0.5, 0, 0);
        myColor(1, 0, 0);
        myVertex(0.5, 0.5, 0);
        myEnd();
    }
};


// Tests zBuffer.
class ScenarioF : public Scenario {
    void display() const {

        myRotate(9, 1, 0.9, 0.8);

        myBegin(GL_TRIANGLES);
        myColor(0, 1, 0);
        myVertex(-1, 1, 1);
        myColor(0, 1, 1);
        myVertex(1, 1, 1);
        myColor(0, 0, 1);
        myVertex(0, -1, -1);
        myEnd();

        myScale(0.5, 0.5, 0.5);

        myBegin(GL_TRIANGLES);
        myColor(1, 1, 0);
        myVertex(-1, -1, 0);
        myColor(1, 0.5, 0);
        myVertex(1, -1, 0);
        myColor(1, 0, 0);
        myVertex(1, 1, 0);
        myEnd();
        myBegin(GL_TRIANGLES);
        myColor(1, 1, 0);
        myVertex(-1, -1, 0);
        myColor(1, 0, 0);
        myVertex(1, 1, 0);
        myColor(1, 0.5, 0);
        myVertex(-1, 1, 0);
        myEnd();

    }
};


// Tests texture mapping.
class ScenarioG : public Scenario {

    // Specific camera for this scene.
    void init(Vector &cam, Vector &focus, bool &persp) const {
        cam = Vector(1.65, 1.25, 1.18);
        focus = Vector(0, 0, 0);
        persp = true;
    }

    void display() const {

        myBindTexture("brick.ppm");

        myTranslate(-0.5, -0.5, -0.5);

        // X+ face of cube
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 1); myVertex(1, 1, 0);
        myTexCoord(0, 0); myVertex(1, 0, 0);
        myTexCoord(1, 1); myVertex(1, 1, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 0); myVertex(1, 0, 0);
        myTexCoord(1, 1); myVertex(1, 1, 1);
        myTexCoord(1, 0); myVertex(1, 0, 1);
        myEnd();

        // Y+ face of cube
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 0); myVertex(1, 1, 0);
        myTexCoord(0, 1); myVertex(0, 1, 0);
        myTexCoord(1, 0); myVertex(1, 1, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 1); myVertex(0, 1, 0);
        myTexCoord(1, 0); myVertex(1, 1, 1);
        myTexCoord(1, 1); myVertex(0, 1, 1);

        myEnd();

        // Z+ face of cube
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 0); myVertex(0, 1, 1);
        myTexCoord(0, 1); myVertex(1, 1, 1);
        myTexCoord(1, 0); myVertex(0, 0, 1);
        myEnd();
        myBegin(GL_TRIANGLES);
        myTexCoord(0, 1); myVertex(1, 1, 1);
        myTexCoord(1, 0); myVertex(0, 0, 1);
        myTexCoord(1, 1); myVertex(1, 0, 1);
        myEnd();

        myBindTexture(NULL);
    }
};


// Tests OBJ reader..
class ScenarioH : public Scenario {

    // Specific camera for this scene.
    void init(Vector &cam, Vector &focus, bool &persp) const {
        cam = Vector(1, 1.5, 3);
        focus = Vector(0, 0, 0);
        persp = true;
    }

    void display() const {
        Object obj = Object::fromFile(objFilename);
        obj.draw();
    }
};


// Tests a convex polygon.
class ScenarioI : public Scenario {
    void display() const {

        myBegin(GL_POLYGON);
        myColor(0, 1, 0);
        myVertex(-0.5, 0, 0);
        myVertex(-0.25, 0.25, 0);
        myVertex(0.25, 0.25, 0);
        myVertex(0.5, 0, 0);
        myVertex(0, -0.75, 0);
        myEnd();
    }
};


// Your scene.
class ScenarioJ : public Scenario {

    // Specific camera for this scene.
    void init(Vector &cam, Vector &focus, bool &persp) const {
        cam = Vector(2, 2.45, 4);
        focus = Vector(0, 0, 0);
        persp = true;
    }

    void display() const {
        // @@@ YOUR SCENE HERE
    }
};


// Registers all scenarios. Add any scenarios you create here.
void initScenarios() {
    scenarios.push_back(new ScenarioA());
    scenarios.push_back(new ScenarioB());
    scenarios.push_back(new ScenarioC());
    scenarios.push_back(new ScenarioD());
    scenarios.push_back(new ScenarioE());
    scenarios.push_back(new ScenarioF());
    scenarios.push_back(new ScenarioG());
    scenarios.push_back(new ScenarioH());
    scenarios.push_back(new ScenarioI());
    scenarios.push_back(new ScenarioJ());
}

