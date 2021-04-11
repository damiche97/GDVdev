#include "Task.h"

#include "TriangleScene.h"
#include <GL/glut.h>      // openGL helper

Task::Task(TaskType t, int numGenBaseTriangles)
  : type(t), numGenBaseTriangles(numGenBaseTriangles)
{ }

void Task::load() {
  // Do we want to randomly render triangles?
  if (type == TaskType::TRIANGLE_GEN) {
    triangleScene(trimesh, numGenBaseTriangles);
  }

  // Otherwise, select a OFF/LSA file depending on the task type and load it
  else {
    const char * filename;
    switch (type) {
      case TaskType::DOLPHIN_LSA:
      case TaskType::DOLPHIN_LSA_ANGLE_NORMALS:
        filename = "Modelle/delphin.lsa";
        break;
      case TaskType::DOLPHIN_OFF:
      case TaskType::DOLPHIN_OFF_ANGLE_NORMALS:
        filename = "Modelle/delphin.off";
        break;
      case TaskType::FORD_LSA:
      case TaskType::FORD_LSA_ANGLE_NORMALS:
        filename = "Modelle/83ford-gt90.lsa";
        break;
      case TaskType::FORD_OFF:
      case TaskType::FORD_OFF_ANGLE_NORMALS:
        filename = "Modelle/83ford-gt90.off";
        break;
    }

    // Depending on the task type, calculate normals with angles
    switch (type) {
      case TaskType::DOLPHIN_OFF_ANGLE_NORMALS:
      case TaskType::DOLPHIN_LSA_ANGLE_NORMALS:
      case TaskType::FORD_OFF_ANGLE_NORMALS:
      case TaskType::FORD_LSA_ANGLE_NORMALS:
        trimesh.setUseAngleNormals(true);
        break;
    }

    switch (type) {
      case TaskType::DOLPHIN_LSA:
      case TaskType::DOLPHIN_LSA_ANGLE_NORMALS:
      case TaskType::FORD_LSA:
      case TaskType::FORD_LSA_ANGLE_NORMALS:
        trimesh.loadLSA(filename);
        break;
        
      case TaskType::DOLPHIN_OFF:
      case TaskType::DOLPHIN_OFF_ANGLE_NORMALS:
      case TaskType::FORD_OFF:
      case TaskType::FORD_OFF_ANGLE_NORMALS:
        trimesh.loadOFF(filename);
        break;
    }
  }
}

void Task::draw() {
  // Draw the triangle mesh.
  // The switch scales down particulary big models, so that they are visible in
  // the viewport. We also try to move them into the (0,0,0) point
  switch (type) {
    case TaskType::FORD_LSA:
    case TaskType::FORD_LSA_ANGLE_NORMALS:
      glPushMatrix();
        glTranslatef(0, 0, 5);
        trimesh.draw();
      glPopMatrix();
      break;
    case TaskType::FORD_OFF:
    case TaskType::FORD_OFF_ANGLE_NORMALS:
      glPushMatrix();
        glEnable(GL_NORMALIZE); // when scaling, we want to auto-adjust normals
        glScalef(0.00005, 0.00005, 0.00005);
        trimesh.draw();
        glDisable(GL_NORMALIZE);
      glPopMatrix();
      break;
    case TaskType::DOLPHIN_LSA:
    case TaskType::DOLPHIN_LSA_ANGLE_NORMALS:
      glPushMatrix();
        glTranslatef(0, 0, 4);
        trimesh.draw();
      glPopMatrix();
      break;
    default:
      trimesh.draw();
      break;
  }
}
