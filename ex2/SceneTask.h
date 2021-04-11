#ifndef SCENE_TASK_H
#define SCENE_TASK_H

#include "Task.h"
#include "TriangleMesh.h"
#include <chrono>

enum class AnimationPhase {
  Drive,
  CrashUp,
  CrashDown,
  Done
};

/**
 * Dynamic scene: A car crashes against a tree
 */
class SceneTask: public Task {
  private:
    TriangleMesh tree_leaves {MeshType::Static}; // Will store triangle mesh for tree leaves
    TriangleMesh tree_trunk {MeshType::Static};  // Will store triangle mesh for tree trunk
    TriangleMesh car {MeshType::Dynamic};        // Mesh for car. Must use VBO streaming, thus set to Dynamic mode.

    // Maximum animation time for each phase of this dynamic scene
    static constexpr const std::chrono::milliseconds maxDriveTime {528};     // drive car for 528 milliseconds
    static constexpr const std::chrono::milliseconds maxCrashUpTime {170};   // crash against tree, rotating upwards for 170ms
    static constexpr const std::chrono::milliseconds maxCrashDownTime {430}; // crash down again for 430ms

    std::chrono::milliseconds movedTime {0};                 // Time the current scene has been running
    std::chrono::milliseconds currentMaxTime {maxDriveTime}; // maximum animation time of the current scene, will be set to one of the above values

    AnimationPhase animPhase {AnimationPhase::Drive}; // current animation phase. Start with the car driving

  public:
    virtual ~SceneTask() = default;

    virtual void load() override;

    virtual void draw(DrawMode mode) override;

    virtual bool tick(std::chrono::milliseconds ms) override;

    virtual void keyPressed(unsigned char key, int x, int y) override;

    virtual unsigned int getNumTriangles() override;
};

#endif
