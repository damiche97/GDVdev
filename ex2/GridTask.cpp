#include "GridTask.h"

#include <iostream>

void GridTask::load() {
  const char * filename = "Models/delphin.off";
  Vec3f mid(0.0f,0.0f,0.0f);

  trimesh.loadOFF(filename, mid, 4.0f);
}

void GridTask::draw(DrawMode mode) {
  // scale scene to fixed box size
  const float scale = 5.0f / max(1,gridSize);
  glScalef(scale, scale, scale);

  for (int i = -gridSize; i <= gridSize; ++i) {
	  for (int j = -gridSize; j <= gridSize; ++j) {
		  if (i != 0 || j != 0) {
			  float r = (float)i/(2.0f*gridSize) + 0.5f;
			  float g = (float)j/(2.0f*gridSize) + 0.5f;
			  float b = 1.0f - 0.5f*r - 0.5f*g;
        glColor3f(r,g,b);
		  }
      else glColor3f(1,1,1);
      glPushMatrix();
      glTranslatef(4.0f*i, 0.0f, 4.0f*j);
	 
      switch (mode) {
        case DrawMode::Array:
          trimesh.drawArray();
          break;

        case DrawMode::VBO:
          trimesh.drawVBO();
          break;

        case DrawMode::Immediate:
          trimesh.drawImmediate();
          break;
      }

      glPopMatrix();
	  }
  }
}

bool GridTask::tick(std::chrono::milliseconds ms) {
  return false; // since changes in time dont affect the scene, dont rerender
}

void GridTask::keyPressed(unsigned char key, int x, int y) {
  using namespace std;

	switch (key) {
    // increase gridSize
    case '+' :
      {
        gridSize++;
        int gridLength = 2*gridSize + 1;
        cout << "Drawing " << gridLength*gridLength*trimesh.getNumTriangles() << " triangles." << endl;
        glutPostRedisplay();
        break;
      }
    // decrease gridSize
    case '-' :
      {
        gridSize--;
        if (gridSize < 0) gridSize = 0;
        int gridLength = 2*gridSize + 1;
        cout << "Drawing " << gridLength*gridLength*trimesh.getNumTriangles() << " triangles." << endl;
        glutPostRedisplay();
        break;
      }
	}
}

unsigned int GridTask::getNumTriangles() {
  const int gridLength = 2*gridSize + 1;

  return gridLength*gridLength*trimesh.getNumTriangles();
}
