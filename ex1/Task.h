#ifndef TASK_H
#define TASK_H

#include "TriangleMesh.h"

// Content that can be rendered by this application.
// See main.cpp for a description of the meaning of the individual types.
enum class TaskType {
  DOLPHIN_LSA,
  DOLPHIN_LSA_ANGLE_NORMALS,
  DOLPHIN_OFF,
  DOLPHIN_OFF_ANGLE_NORMALS,
  FORD_LSA,
  FORD_LSA_ANGLE_NORMALS,
  FORD_OFF,
  FORD_OFF_ANGLE_NORMALS,
  TRIANGLE_GEN
};

/**
 * Manages which content should be rendered based on the TaskType given in the
 * constructor
 */
class Task {
  private:
    // Content type to be rendered
    TaskType type;
    // Number of triangles / 3 to be rendered when choosing
    // TaskType::TRIANGLE_GEN. It is ignored if a different type is choosen.
    int numGenBaseTriangles;
    TriangleMesh trimesh;

  public:
    // See above comments for parameter description
    Task(TaskType t, int numGenBaseTriangles = 1000);

    /**
     * Load content from LSA/OFF file or generate triangle mesh randomly
     * depending on selected TaskType
     */
    void load();

    /**
     * Draw loaded content.
     * This method may apply some scaling depending on the content so that
     * it is visible in the viewport
     */
    void draw();
};

#endif
