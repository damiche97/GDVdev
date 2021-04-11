#include "SceneTask.h"
#include <iostream>

void SceneTask::load() {
  tree_leaves.loadOFF("Models/tree_leaves.off");

  tree_trunk.loadOFF("Models/tree_trunk.off");

  car.loadOFF("Models/car.off");
    car.translateToCenter({ 0, 1.5f, 30.0f });
}

void SceneTask::draw(DrawMode mode) {
  constexpr const float trunkColor[] { 0.427, 0.298, 0.255 };
  constexpr const float carColor[] { 0.62, 0.62, 0.62 };
  constexpr const float leavesColor[] { 0.18, 0.49, 0.196 };
  constexpr const float neutralColor[] { 1.0, 1.0, 1.0 };

  switch (mode) {
    case DrawMode::Immediate:
      glColor3fv(leavesColor);
      tree_leaves.drawImmediate();

      glColor3fv(trunkColor);
      tree_trunk.drawImmediate();

      glColor3fv(carColor);
      car.drawImmediate();
      break;
    case DrawMode::Array:
      glColor3fv(leavesColor);
      tree_leaves.drawArray();

      glColor3fv(trunkColor);
      tree_trunk.drawArray();

      glColor3fv(carColor);
      car.drawArray();
      break;
    case DrawMode::VBO:
      glColor3fv(leavesColor);
      tree_leaves.drawVBO();

      glColor3fv(trunkColor);
      tree_trunk.drawVBO();

      glColor3fv(carColor);
      car.drawVBO();
      break;
  }

  glColor3fv(neutralColor);
}

bool SceneTask::tick(std::chrono::milliseconds ms) {

  std::chrono::milliseconds moveTime; // time we can move in the current phase
  std::chrono::milliseconds remainingTime; // additional moving time which exceeds the maximum time of the current phase
  const bool switchPhase = movedTime + ms >= currentMaxTime; // whether we should switch to the next phase, if the time of the current one elapsed

  // If more time has passed than allocated for the current phase...
  if (movedTime + ms > currentMaxTime) {
    moveTime = currentMaxTime - movedTime;           // calculate the part of it which still belongs to this phase
    remainingTime = movedTime + ms - currentMaxTime; // and the part belonging to the next phase

    movedTime = std::chrono::milliseconds::zero();   // reset the time accumulator
  }

  else { // Otherwise...
    moveTime = ms; // just move for the given ammount of time
    remainingTime = std::chrono::milliseconds::zero();
    movedTime += ms; // and record it
  }

  switch (animPhase) { // depending on the current phase, a different animation is chosen
    case AnimationPhase::Drive:
      {
        auto movedDistance = -0.05 * moveTime.count();
        const Matrix carMovement {
          {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, movedDistance
          }
        };
        car.applyMatrix(carMovement);

        if (switchPhase) {
          animPhase = AnimationPhase::CrashUp;
          currentMaxTime = maxCrashUpTime;
          movedTime = std::chrono::milliseconds::zero();
        }

        break;
      }
    case AnimationPhase::CrashUp:
      {
        // Upward motion
        const auto angularVelocity = -0.005; // radiants per milliseconds

        auto angle = angularVelocity * moveTime.count();

        const auto genTiltMatrix = [](double angle) -> Matrix {
          return {
            {
              1, 0, 0, 0,
              0, cos(angle), -sin(angle), 0,
              0, sin(angle), cos(angle), 0,
            }
          };
        };

        const auto carTilt = genTiltMatrix(angle);

        // Sideway motion
        const auto sidewayVelocity = -0.002; // radiants per milliseconds

        angle = sidewayVelocity * moveTime.count();
        const auto genSidewayTiltMatrix = [](double angle) -> Matrix {
          return {
            {
               cos(angle), 0, sin(angle), 0,
                        0, 1,          0, 0,
              -sin(angle), 0, cos(angle), 0,
            }
          };
        };

        const auto sidewayMotion = genSidewayTiltMatrix(angle);

        car.applyMatrix(carTilt * sidewayMotion);

        if (switchPhase) {
          animPhase = AnimationPhase::CrashDown;
          currentMaxTime = maxCrashDownTime;
          movedTime = std::chrono::milliseconds::zero();
        }

        break;
      }
    case AnimationPhase::CrashDown:
      {
        const auto angularVelocity = 0.002;

        const auto angle = angularVelocity * moveTime.count();
        const auto genTiltMatrix = [](double angle) -> Matrix {
          return {
            {
              1, 0, 0, 0,
              0, cos(angle), -sin(angle), 0,
              0, sin(angle), cos(angle), 0,
            }
          };
        };

        const auto carTilt = genTiltMatrix(angle);

        car.applyMatrix(carTilt);

        if (switchPhase) {
          animPhase = AnimationPhase::Done;
          movedTime = std::chrono::milliseconds::zero();
        }

        break;
      }
    case AnimationPhase::Done:
      return false; // Do not rerender, when done
  }

  if (remainingTime > std::chrono::milliseconds::zero()) {
    this->tick(remainingTime);
  }

  return true; // always rerender, if not in "Done" phase
}

void SceneTask::keyPressed(unsigned char key, int x, int y) {
}

unsigned int SceneTask::getNumTriangles() {
  return car.getNumTriangles() + tree_leaves.getNumTriangles() + tree_trunk.getNumTriangles();
}
