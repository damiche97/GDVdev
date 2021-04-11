#ifndef GRID_TASK_H
#define GRID_TASK_H

#include "Task.h"
#include "TriangleMesh.h"

/**
 * Original scene of this template: Dolphins in a grid
 */
class GridTask: public Task {
  private:
    TriangleMesh trimesh {MeshType::Static};
    int gridSize {3};

  public:
    virtual ~GridTask() override = default;

    virtual void load() override;

    virtual void draw(DrawMode mode) override;

    virtual bool tick(std::chrono::milliseconds ms) override;

    virtual void keyPressed(unsigned char key, int x, int y) override;

    virtual unsigned int getNumTriangles() override;
};

#endif
