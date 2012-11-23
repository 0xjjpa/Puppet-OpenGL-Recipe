#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cfloat>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "linalg.hpp"
#include "mygl.hpp"
#include "object.hpp"


// Initilize the object cache.
std::map<std::string,Object> Object::cache_;


void Vertex::draw(Object const &obj) const {
    // Indices can be -1 to indicate that data does not exist.
    // We don't do anything with normals because there isn't a myNormal.
    if (ti_ >= 0) {
        myTexCoord(obj.texCoords_[ti_][0], obj.texCoords_[ti_][1]);
    }
    if (ci_ >= 0) {
        myColor(obj.colors_[ci_][0], obj.colors_[ci_][1], obj.colors_[ci_][2]);
    }
    if (ni_ >= 0) {
        myNormal(obj.normals_[ni_][0], obj.normals_[ni_][1], obj.normals_[ni_][2]);
    }
    myVertex(obj.positions_[pi_][0], obj.positions_[pi_][1], obj.positions_[pi_][2]);
}


bool Object::readOBJ(std::string const &filename) {

    // Try to open the file.
    std::ifstream file(filename.c_str());
    if (!file.good()) {
        std::cerr << "Unable to open OBJ file \"" << filename << "\"" << std::endl;
        return false;
    }

    // Keep fetching op codes and processing them. We will assume that there
    // is one operation per line.
    while (file.good()) {

        std::string opString;
        std::getline(file, opString);

        std::stringstream opStream(opString);
        std::string opCode;
        opStream >> opCode;

        // Skip blank lines and comments
        if (!opCode.size() || opCode[0] == '#') {
            continue;
        }

        // Ignore groups.
        if (opCode[0] == 'g') {
            std::cerr << "ignored OBJ opCode '" << opCode << "'" << std::endl;

        // Vertex data.
        } else if (opCode[0] == 'v') {

            // Read in up to 4 doubles.
            Vector vec;
            for (int i = 0; opStream.good() && i < 4; i++) {
                double value;
                opStream >> vec[i];
            }

            // Store this data in the right location.
            switch (opCode.size() > 1 ? opCode[1] : 'v') {
                case 'v':
                    positions_.push_back(vec);
                    break;
                case 't':
                    texCoords_.push_back(vec);
                    break;
                case 'n':
                    normals_.push_back(vec);
                    break;
                case 'c':
                    colors_.push_back(vec);
                    break;
                default:
                    std::cerr << "unknown vertex type '" << opCode << "'" << std::endl;
                    break;
            }

        // A polygon (or face).
        } else if (opCode == "f") {
            std::vector<Vertex> polygon;
            // Limit to 4 as we only can handle triangles and quads.
            for (int i = 0; opStream.good() && i < 4; i++) {

                // Retrieve a full vertex specification.
                std::string vertexString;
                opStream >> vertexString;

                // Parse the vertex into a set of indices for position,
                // texCoord, normal, and colour, respectively.
                std::stringstream vertexStream(vertexString);
                std::vector<int> indices;
                for (int j = 0; vertexStream.good() && j < 4; j++) {
                    // Skip slashes.
                    if (vertexStream.peek() == '/') {
                        vertexStream.ignore(1);
                    }
                    int index;
                    vertexStream >> index;
                    indices.push_back(index);
                }

                // Turn this into a real Vertex, and append it to the polygon.
                indices.resize(4, 0);
                polygon.push_back(Vertex(
                    indices[0] - 1,
                    indices[1] - 1,
                    indices[2] - 1,
                    indices[3] - 1
                ));

            }

            // Only accept triangles and quads.
            if (polygon.size() >= 3) {
                polygons_.push_back(polygon);
            }

        // Any other opcodes get ignored.
        } else {
            std::cerr << "unknown opCode '" << opCode << "'" << std::endl;
        }
    }

    return true;

}


void Object::fillNormals() {
    // Walk through all of the polygons...
    for (int i = 0; i < polygons_.size(); i++) {
        std::vector<Vertex> &polygon = polygons_[i];
        // ... and if it doesn't have normals set...
        if (polygon[0].ni_ < 0) {
            // ... calculate one...
            Vector const &a = positions_[polygon[0].pi_];
            Vector const &b = positions_[polygon[1].pi_];
            Vector const &c = positions_[polygon[2].pi_];
            Vector normal = (b - a).cross(c - a);
            normal.normalize();
            // ... and set all of the verticies to use it.
            normals_.push_back(normal);
            for (int j = 0; j < polygon.size(); j++) {
                polygon[j].ni_ = normals_.size() - 1;
            }
        }
    }
}


void Object::fillColors() {
    // Walk through all of the polygons...
    for (int i = 0; i < polygons_.size(); i++) {
        std::vector<Vertex> &polygon = polygons_[i];
        // ... and all of their vertices.
        for (int j = 0; j < polygon.size(); j++) {
            // If they don't have a color, but do have a normal...
            if (polygon[j].ci_ < 0 && polygon[j].ni_ >= 0) {
                // Set their color based off the normal.
                Vector color = (normals_[polygon[j].ni_] + Vector(1, 1, 1)) * 0.5;
                colors_.push_back(color);
                polygon[j].ci_ = colors_.size() - 1;
            }
        }
    }

}


void Object::normalize() {

    // Calculate the bounding box.
    Vector minCoord(DBL_MAX, DBL_MAX, DBL_MAX);
    Vector maxCoord(DBL_MIN, DBL_MIN, DBL_MIN);
    for (int i = 0; i < positions_.size(); i++) {
        for (int j = 0; j < 3; j++) {
            minCoord[j] = std::min(minCoord[j], positions_[i][j]);
            maxCoord[j] = std::max(maxCoord[j], positions_[i][j]);
        }
    }


    // Calculate the center of the box.
    Vector offset = (minCoord + maxCoord) / 2.0 ;

    // Calculate a scale that brings us to an average unit size.
    double scale = 0;
    for (int i = 0; i < 3; i++) {
        scale += maxCoord[i] - minCoord[i];
    }
    scale = 2.0 / (scale / 3.0);

    // Transform all of the positional data with these values.
    for (int i = 0; i < positions_.size(); i++) {
        for (int j = 0; j < 3; j++) {
            positions_[i][j] -= offset[j];
            positions_[i][j] *= scale;
        }
    }
}



Object& Object::fromFile(std::string const &filename) {
    // Retrieve the object from the cache.
    Object &obj = Object::cache_[filename];
    // Load it if it isn't already.
    if (!obj.good()) {
        obj.readOBJ(filename);
        obj.normalize();
        obj.fillNormals();
        obj.fillColors();
    }
    return obj;
}


void Object::draw() const {

    // Walk across all of the polygons.
    for (int poly_i = 0; poly_i < polygons_.size(); poly_i++) {
        std::vector<Vertex> const *polygon = &polygons_[poly_i];

        // Draw triangles directly.
        if (polygon->size() == 3) {
            myBegin(GL_TRIANGLES);
            for (int i = 0; i < polygon->size(); i++) {
                (*polygon)[i].draw(*this);
            }
            myEnd();

        // Split quads into two triangles.
        } else if (polygon->size() == 4) {
            myBegin(GL_TRIANGLES);
            (*polygon)[0].draw(*this);
            (*polygon)[1].draw(*this);
            (*polygon)[2].draw(*this);
            myEnd();
            myBegin(GL_TRIANGLES);
            (*polygon)[2].draw(*this);
            (*polygon)[3].draw(*this);
            (*polygon)[0].draw(*this);
            myEnd();
        }
    }
}

