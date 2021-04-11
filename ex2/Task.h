#ifndef TASK_H
#define TASK_H

#include <chrono>

#include "TriangleMesh.h"

enum class DrawMode: int {
  Immediate = 0,
  Array = 1,
  VBO = 2
};

/**
 * Represents a scene to be rendered
 */
class Task {
  public:
    virtual ~Task() = default;

    // Load models
    virtual void load() = 0;

    // Draw scene
    virtual void draw(DrawMode mode) = 0;

    /**
     * To be called to inform the scene that time has passed.
     * This is used for animation.
     * It shall return, whether something changed in the time frame and the
     * scene must be rerendered.
     */
    virtual bool tick(std::chrono::milliseconds ms) = 0;

    // Handle input
    virtual void keyPressed(unsigned char key, int x, int y) = 0;

    // Report triangles in scene
    virtual unsigned int getNumTriangles() = 0;
};

#endif
