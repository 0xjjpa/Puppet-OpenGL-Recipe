#ifndef SCENARIO_H
#define SCENARIO_H

#include <vector>

#include "linalg.hpp"


// A class to control initial camera state of a scenario, and how to draw it.
class Scenario {
public:

    // Initialize the camera to use the given position, focus, and projection.
    virtual void init(
        Vector &cameraPosition,
        Vector &cameraFocus,
        bool &usePerspective
    ) const;

    // Display the scene.
    virtual void display() const = 0;

};


// The list of scenarios.
extern std::vector<Scenario*> scenarios;


// The function which registers all scenarios into the scenario vector.
void initScenarios();


#endif
