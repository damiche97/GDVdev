// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple and extendable openGL program offering                    //
//   * basic navigation and basic pipeline rendering                         //
// ========================================================================= //

#include <GL/glew.h>      // openGL helper
#include <GL/glut.h>      // openGL helper

#include <stdlib.h>       // namespace std
#include <math.h>         // fmod
#include <time.h>         // clock_t
#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file
#include <algorithm>	  // min, max
#include <limits>
#include <chrono>
#include <vector>
#include <optional>

#include "imageloader.h"  // simple class for loading bmp files

#include "SceneLoader.h"

#include "main.h"

// FIXME: These are used for setting debugging visuals, remove later!
std::vector<Vec3f> hitSpheres;
std::vector<Vec3f> castSpheres;
std::vector<std::pair<Vec3f, Vec3f>> rays;
std::vector<std::pair<Vec3f, Vec3f>> rays2;
std::vector<Vec3f> markers;

bool showMarks = false;

// ==============
// === BASICS ===
// ==============

constexpr const auto shadowEpsilon = 0.0001f;
constexpr const auto rayOffset = 0.0001f;

int main(int argc, char** argv) {
  // initialize openGL window
  glutInit(&argc, argv);
  glutInitWindowPosition(300,200);
  glutInitWindowSize(300,300);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("OpenGL Raytracer");

  // link functions to certain openGL events
  glutDisplayFunc(renderScene);
  glutReshapeFunc(reshape);
  glutMouseFunc(mousePressed);
  glutMotionFunc(mouseMoved);
  glutKeyboardFunc(keyPressed);
  glutKeyboardUpFunc(keyReleased);
  // further initializations
  initialize();
  setDefaults();

  // set light intensity
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensityAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensityDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightIntensitySpecular);
  // load meshes
  string filename;
  // load textures
  Image* image;
  filename = "Textures/TEST_GRID.bmp";
  image = loadBMP(filename.c_str());
  textureIDs.push_back(loadTexture(image));

  int scene_number = 1;
  if (argc > 1) {
    scene_number = std::stoi(argv[1]);
  }

  // load scene
  switch (scene_number) {
    // experiment scene
    case 0:
      lightPos.set(-6.0f, 0.5f, 0.0f);

      load(
        "Models/seahorse.off",
        ModelScaling { 4.f },
        [](TriangleMesh & tm) {
          tm.rotX(180);
          tm.rotY(20);
          tm.rotZ(180);
          tm.translate({-1.f, -1.f, -3.5f});
        },
        TextureID { 1 },
        MatAmbient { 0.1f, 0.2f, 0.1f },
        MatDiffuse { 0.3f, 0.6f, 0.3f },
        MatSpecular { 0.4f, 0.4f, 0.4f, }
      );

      load(
        "Models/seahorse.off",
        ModelScaling { 2.f },
        [](TriangleMesh & tm) {
          tm.rotX(180);
          tm.rotY(20);
          tm.rotZ(180);
          tm.translate({-2.f, 1.f, 3.5f});
        },
        TextureID { 1 },
        MatAmbient { 0.1f, 0.2f, 0.1f },
        MatDiffuse { 0.3f, 0.6f, 0.3f },
        MatSpecular { 0.4f, 0.4f, 0.4f, }
      );

      load(
        "Models/seahorse.off",
        ModelScaling { 2.5f },
        [](TriangleMesh & tm) {
          tm.rotX(180);
          tm.rotY(20);
          tm.rotZ(160);
          tm.translate(Vec3f {-5.0f, 1.0f, 3.5f});
        },
        NoTexture,
        MatAmbient { 0.2f, 0.2f, 0.4f },
        MatDiffuse { 0.1f, 0.1f, 0.3f },
        MatSpecular { 0.2f, 0.2f, 0.4f },
        MatReflectiveness { 0.6f, 0.6f, 1.0f }
      );

      load(
        "Models/seahorse.off",
        ModelScaling { 2.6f },
        [](TriangleMesh & tm) {
          tm.rotX(180);
          tm.rotY(20);
          tm.rotZ(90);
          tm.translate(Vec3f {-3.f, -1.2f, -0.5f});
        },
        NoTexture,
        MatAmbient { 0.3f, 0.3f, 0.3f, },
        MatDiffuse { 0.0f, 0.0f, 0.0f, },
        MatSpecular { 0.0f, 0.0f, 0.0f, },
        MatReflectiveness { 0.1f, 0.1f, 0.1f, },
        MatOpacity { 0.5f, 0.5f, 0.5f },
        MatRefractiveIndex { 1.8 }
      );

      load(
        "Models/seahorse.off",
        ModelScaling { 1.7f },
        [](TriangleMesh & tm) {
          tm.rotX(180);
          tm.rotY(20);
          tm.rotZ(200);
          tm.translate({-3.f, -1.65f, 4.f});
        },
        TextureID { 1 },
        MatAmbient { 0.3f, 0.3f, 0.3f, },
        MatDiffuse { 0.0f, 0.0f, 0.0f, },
        MatSpecular { 0.0f, 0.0f, 0.0f, },
        MatReflectiveness { 0.0f, 0.0f, 0.0f, },
        MatOpacity { 0.5f, 0.2f, 0.2f },
        MatRefractiveIndex { 1.0f }
      );

      load(
        "Models/floor.off",
        ModelScaling { 20.f },
        [](TriangleMesh & tm) {
          tm.translate({0.f, -2.f, 0.f});
        },
        TextureID { 1 },
        MatAmbient { 0.8f, 0.8f, 0.8f },
        MatDiffuse { 0.9f, 0.9f, 0.9f },
        MatSpecular { 0.10f, 0.10f, 0.10f }
      );

      load( // mirror
        "Models/floor.off",
        ModelScaling { 8.f },
        [](TriangleMesh & tm) {
          tm.rotX(90);
          tm.rotY(20);
          tm.translate({-4.f, 0.f, -5.f});
        },
        NoTexture,
        MatAmbient { 0.0f, 0.0f, 0.0f },
        MatDiffuse { 0.03f, 0.03f, 0.03f },
        MatSpecular { 0.3f, 0.3f, 0.3f },
        MatReflectiveness { 1.0f, 1.0f, 1.0f }
      );
      break;

    // balloon scene
    case 1:
      load(
        "Models/ballon.off",
        ModelScaling { 20.f },
        NoTransform,
        TextureID { 1 },
        MatAmbient { 0.2f, 0.1f, 0.1f },
        MatDiffuse { 0.6f, 0.3f, 0.3f },
        MatSpecular { 0.4f, 0.4f, 0.4f }
      );

      load(
        "Models/delphin.off",
        ModelScaling { 3.0f },
        [](TriangleMesh & tm) {
          tm.rotY(50);
          tm.translate(Vec3f{5.0, 0.0, 2.0});
        },
        NoTexture,
        MatAmbient { 0.1f, 0.2f, 0.1f },
        MatDiffuse { 0.3f, 0.6f, 0.3f },
        MatSpecular { 0.4f, 0.4f, 0.4f, }
      );

      load(
        "Models/cylinder.off",
        ModelScaling { 2.5f },
        [](TriangleMesh & tm) {
          tm.translate(Vec3f {-5.0f, 0.0f, 4.5f});
        },
        NoTexture,
        MatAmbient { 0.3f, 0.3f, 0.3f },
        MatDiffuse { 0.2f, 0.2f, 0.2f },
        MatSpecular { 0.3f, 0.3f, 0.3f },
        MatReflectiveness { 1.0f, 1.0f, 1.0f }
      );

      load(
        "Models/sphere.off",
        ModelScaling { 2.f },
        [](TriangleMesh & tm) {
          tm.translate(Vec3f {4.0f, 0.0f, 25.f});
        },
        NoTexture,
        MatAmbient { 0.3f, 0.3f, 0.3f, },
        MatDiffuse { 0.0f, 0.0f, 0.0f, },
        MatSpecular { 0.0f, 0.0f, 0.0f, },
        MatReflectiveness { 0.0f, 0.0f, 0.0f, },
        MatOpacity { 0.5f },
        MatRefractiveIndex { 1.2 }
      );
      break;

    default:
      std::cout << "There is no scene with this number. Aborting." << std::endl;
      exit(1);
  }

  for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].coutData();

  // activate main loop
  coutHelp();
  glutIdleFunc(handleMovement);
  glutMainLoop();
  return 0;
}

void initialize() {
  // black screen
  glClearColor(0,0,0,0);
  // initialize glew
  GLenum err = glewInit();
  if (GLEW_OK != err) cout << "GLEW Error (" << err << "): " << glewGetErrorString(err) << endl;
  cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << endl;
  // check if VBOs are supported
  if(glGenBuffers && glBindBuffer && glBufferData && glBufferSubData &&
    glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv)
    cout << "VBOs are supported!" << endl;
  else cout << "VBOs are NOT supported!" << endl;
  // set shading model
  glShadeModel(GL_SMOOTH);
  // activate depth tests
  glEnable(GL_DEPTH_TEST);
  // enable light with defaults
  glEnable(GL_LIGHT0);
}

void setDefaults() {
  lightIntensityAmbient[0]    = 0.2f;
    lightIntensityAmbient[1]  = 0.2f;
    lightIntensityAmbient[2]  = 0.2f;
    lightIntensityAmbient[3]  = 1.0f;

  lightIntensityDiffuse[0]    = 1.0f;
    lightIntensityDiffuse[1]  = 1.0f;
    lightIntensityDiffuse[2]  = 1.0f;
    lightIntensityDiffuse[3]  = 1.0f;

  lightIntensitySpecular[0]   = 1.0f;
    lightIntensitySpecular[1] = 1.0f;
    lightIntensitySpecular[2] = 1.0f;
    lightIntensitySpecular[3] = 1.0f;

  // scene Information
  cameraPos.set(0,0,10);
  cameraDir.set(0,0,-1);
  movementSpeed = 0.02f;
  // light information
  lightPos.set(0.0f, 0.0f, 8.0f);
  lightMotionSpeed = -0.05f;
  moveLight = false;
  // mouse information
  mouseButton = 0;
  mouseSensitivy = 1.0f;
  // key information, all false
  keyDown.resize(256,false);
  // draw mode (VBO)
  drawMode = 0;

  max_recursion_depth = 4;
}

void reshape(const GLint width, const GLint height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (float)width / (float)height, 1, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Variable to track the progress of time for a scene.
// We are using optionals so that we can simply reset time by assigning
// std::nullopt to this variable.
std::optional<
  std::chrono::time_point< // We are using the C++ chrono library, since it provides nice type safe tools to deal with time
    std::chrono::system_clock // we are measuring time with the standard system clock
  >
> last_render_time = std::nullopt;

void handleMovement() {
  const auto now = std::chrono::system_clock::now();

  std::chrono::system_clock::duration time_delta;
  if (last_render_time.has_value()) {
    time_delta = now - *last_render_time;
  }
  else {
    time_delta = std::chrono::system_clock::duration(0);
  }

  last_render_time.emplace(now);

  const auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(time_delta).count();

  // perform light movement and trigger redisplay
  if (moveLight == true) {
    lightPos.rotY(lightMotionSpeed*msPassed);
    glutPostRedisplay();
  }
  // first person movement
  if (keyDown[(int)'w'] || keyDown[(int)'W']) {
    cameraPos += movementSpeed*cameraDir*(float)msPassed;
    glutPostRedisplay();
  }
  if (keyDown[(int)'s'] || keyDown[(int)'S']) {
    cameraPos -= movementSpeed*cameraDir*(float)msPassed;
    glutPostRedisplay();
  }
  if (keyDown[(int)'a'] || keyDown[(int)'A']) {
    Vec3f ortho(-cameraDir.z,0.0f,cameraDir.x);
    ortho.normalize();
    cameraPos -= movementSpeed*ortho*(float)msPassed;
    glutPostRedisplay();
  }
  if (keyDown[(int)'d'] || keyDown[(int)'D']) {
    Vec3f ortho(-cameraDir.z,0.0f,cameraDir.x);
    ortho.normalize();
    cameraPos += movementSpeed*ortho*(float)msPassed;
    glutPostRedisplay();
  }
  if (keyDown[(int)'q'] || keyDown[(int)'Q']) {
    cameraPos.y += movementSpeed*(float)msPassed;
    glutPostRedisplay();
  }
  if (keyDown[(int)'e'] || keyDown[(int)'E']) {
    cameraPos.y -= movementSpeed*(float)msPassed;
    glutPostRedisplay();
  }
}

// =================
// === RENDERING ===
// =================

void drawCS() {
  glBegin(GL_LINES);
    // red X
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(5,0,0);
    // green Y
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,5,0);
    // blue Z
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,5);
  glEnd();
}

void drawLight() {
  GLfloat lp[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, lp);
  // draw yellow sphere for light source
  glPushMatrix();
    glTranslatef(lp[0], lp[1], lp[2]);
    glColor3f(1,1,0);
    glutSolidSphere(0.2f,16,16);
  glPopMatrix();
}

void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // render ray trace result on a quad in front of the camera
  if ( raytracedTextureID ) {
    // save matrix and load identities => no transformation, no projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // render textured quad in the middle of the view frustum (depth 0) filling complete view window
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, raytracedTextureID);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex3d(-1.0,-1.0,0);
    glTexCoord2d(1.0,0.0); glVertex3d( 1.0,-1.0,0);
    glTexCoord2d(1.0,1.0); glVertex3d( 1.0, 1.0,0);
    glTexCoord2d(0.0,1.0); glVertex3d(-1.0, 1.0,0);
    glEnd();
    // restore matrix
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_2D);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
  // else, standard openGL rendering
  else {
    // clear and set camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vec3f cameraLookAt = cameraPos + cameraDir;
    gluLookAt(cameraPos.x,    cameraPos.y,    cameraPos.z,    // Position
              cameraLookAt.x, cameraLookAt.y, cameraLookAt.z, // Lookat
              0.0,            1.0,            0.0);           // Up-direction
    // draw coordinate system without lighting
    glDisable(GL_LIGHTING);
    drawCS();
    // draw sphere for light still without lighting
    drawLight();

    // FIXME: Draw debugging visuals, remove later
    if (showMarks) {
      for (auto const & spherePos : hitSpheres) {
        glPushMatrix();
          glTranslatef(spherePos[0], spherePos[1], spherePos[2]);
          glColor3f(1,1,0);
          glutSolidSphere(0.1f,3,3);
        glPopMatrix();
      }

      for (auto const & spherePos : castSpheres) {
        glPushMatrix();
          glTranslatef(spherePos[0], spherePos[1], spherePos[2]);
          glColor3f(0.3,0.3,1);
          glutSolidSphere(0.1f,3,3);
        glPopMatrix();
      }

      for (auto const & spherePos : markers) {
        glPushMatrix();
          glTranslatef(spherePos[0], spherePos[1], spherePos[2]);
          glColor3f(1,0,0);
          glutSolidSphere(0.05f,3,3);
        glPopMatrix();
      }

      glColor3f(1,0,1);
      glBegin(GL_LINES);
      for (auto const & ray : rays) {
        glVertex4f(ray.first.x, ray.first.y, ray.first.z, 1.0);
        glVertex4f(ray.second.x, ray.second.y, ray.second.z, 1.0);
      }
      glEnd();

      glColor3f(1,1,0);
      glBegin(GL_LINES);
      for (auto const & ray : rays2) {
        glVertex4f(ray.first.x, ray.first.y, ray.first.z, 1.0);
        glVertex4f(ray.second.x, ray.second.y, ray.second.z, 1.0);
      }
      glEnd();
    }

    // draw object
    glEnable(GL_LIGHTING);
    for (unsigned int i = 0; i < meshes.size(); i++) {
      // apply materials and texture
      glMaterialfv(GL_FRONT, GL_AMBIENT, objects[i].matAmbient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, objects[i].matDiffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, objects[i].matSpecular);
      glMaterialf(GL_FRONT, GL_SHININESS, objects[i].matShininess);
      glBindTexture(GL_TEXTURE_2D, objects[i].textureID.value_or(0));
      // draw
      meshes[i].draw();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // swap Buffers
  glutSwapBuffers();
}

float calcLight(
    SceneObject const & hitObject,
    Vec3f const & L,
    Vec3f const & N,
    Vec3f const & H,
    std::size_t component
) {
  return
      lightIntensityDiffuse[component] * hitObject.matDiffuse[component] * (L*N)
    + lightIntensitySpecular[component] * hitObject.matSpecular[component] * pow((H*N), hitObject.matShininess);
}

bool addMarkers = false;

// indicates, whether ray can reach light
bool hitsLight(const Ray<float> & ray, float lightDistance /*should be set to light distance*/) {
  float t = lightDistance;
  float u, v;

  // iterate over all meshes
  for (unsigned int i = 0; i < meshes.size(); ++i) {
    // check ray versus bounding box first
    //if (!rayAABBIntersect(ray, meshes[i].boundingBoxMin, meshes[i].boundingBoxMax, 0.0f, t)) {
    //  continue;
    //}

    // get triangle information
    vector<Vec3f> const & vertices   = meshes[i].getVertices();
    vector<Vec3ui> const & triangles = meshes[i].getTriangles();

    // brute force: iterate over all triangles of the mesh
    for (unsigned int j = 0; j < triangles.size(); j++) {
      Vec3f const & p0 = vertices[triangles[j][0]];
      Vec3f const & p1 = vertices[triangles[j][1]];
      Vec3f const & p2 = vertices[triangles[j][2]];

      // check if triangle is hit by ray
      const bool hit = ray.triangleIntersect(&p0.x, &p1.x, &p2.x, u, v, t);
      ++intersectionTests;

      // if so, we can record which triangle was hit and return
      if (
             hit
          && t > shadowEpsilon /* dont register hits, which are too close. object might otherwise cast shadow on itself. */
          && t < lightDistance
      ) {
        return false;
      }
    }
  }

  return true;
}

bool isMiddle = false;

/**
 * Berechne den ausgehenden Vektor, der aus einer Lichtbrechung zwischen zwei
 * Medien hervorgeht.
 *
 * Quelle Formeln: "Ray Tracing from the Ground Up" - ISBN 978-1-56881-272-4
 */
Vec3f calculateTransmissionVector(
    float eta, // Brechungsindex
    Ray<float> const & ray, // incoming ray
    Vec3f const & N // normal vector
) {
  const auto omega0 =  ray.d;
  const float cosThetaI = N * omega0;
  const float cosThetaT = sqrt(
    1 - (1 - cosThetaI*cosThetaI)/(eta*eta)
  );

  return omega0 / eta - (cosThetaT - cosThetaI/eta) * N;
}

/**
 * Calculates whether a given ray hits a triangle within an object.
 * It differs from the other hit calculation functions, in that it only
 * checks for hits with a given object and flips all its faces for hit detection.
 *
 * It is used for tracking collisions in a transparent medium with its boundaries.
 *
 * @return boolean indicating whether something had been hit.
 * @param mesh mesh of the object with whose faces collisions shall be checked
 * @param ray ray which supposedly collides with the mesh
 * @param hitPoint if a face is hit, the point of impact will be stored in this
 *   output parameter
 * @param hitNormal if a face is hit, the interpolated inward normal of the
 *   point where the face was hit is stored in this output parameter
 */
bool calculateHitInMedium(
    TriangleMesh const & mesh,
    Ray<float> const & ray,

    Vec3f & hitPoint,
    Vec3f & hitNormal
) {
  // We will cache the results for the closest hit found so far in these variables
  auto closestT = std::numeric_limits<float>::max(); // set it initially to the greatest value possible, so that any possible hit to be found is smaller
  float closestU = 0;
  float closestV = 0;
  unsigned int closestHitTriIdx = 0;

  // indicates, if we hit anything
  bool hitSomething = false;

  // get triangle information
  vector<Vec3f> const & vertices   = mesh.getVertices();
  vector<Vec3ui> const & triangles = mesh.getTriangles();

  {
    float t = 1000.f;
    float u;
    float v;

    // brute force: iterate over all triangles of the mesh
    for (unsigned int j = 0; j < triangles.size(); j++) {
      // (note, that we use a reverse ordering for the triangle vertices
      //   to flip its face direction)
      Vec3f const & p0 = vertices[triangles[j][0]];
      Vec3f const & p1 = vertices[triangles[j][2]];
      Vec3f const & p2 = vertices[triangles[j][1]];

      // check if triangle is hit by ray
      const bool hit = ray.triangleIntersect(&p0.x, &p1.x, &p2.x, u, v, t);
      ++intersectionTests;

      // if so, we can record which triangle was hit and return
      if (hit && t > 0.0f) {
        if (t < closestT) {
          closestT = t;

          closestU = u;
          closestV = v;
          closestHitTriIdx = j;

          hitSomething = true;
        }
      }
    }
  }

  if (hitSomething) {
    auto const & hitTriangle = triangles[closestHitTriIdx];

    // (note, that we use a reverse ordering for the triangle vertices
    //   to flip its face direction)
    auto const & hitP0 = vertices[hitTriangle[0]];
    auto const & hitP1 = vertices[hitTriangle[2]];
    auto const & hitP2 = vertices[hitTriangle[1]];

    // flipping normals..
    auto const & normals = mesh.getNormals();
    const auto hitN0 = -1.0f * normals[hitTriangle[0]];
    const auto hitN1 = -1.0f * normals[hitTriangle[2]];
    const auto hitN2 = -1.0f * normals[hitTriangle[1]];

    const auto w = 1-closestU-closestV;
    hitPoint = w*hitP0 + closestU*hitP1 + closestV*hitP2;
    hitNormal = (w*hitN0 + closestU*hitN1 + closestV*hitN2).normalized();
  }

  return hitSomething;
}

/**
 * Checks whether a ray collision within a transparent medium with its boundaries
 * should result in a total internal reflection.
 *
 * Source of formulas: "Ray Tracing from the Ground Up" - ISBN 978-1-56881-272-4
 */
bool isTotalInternalReflection(
    float cosThetaI, // scalar product of reversed incoming ray and normal
    float refractiveIndex // refractive index of medium we are in
) {
  // we are multiplying here with the refractive index instead of dividing,
  // since we have to inverse it, if we are in the medium
  return 1 - (1 - cosThetaI*cosThetaI)*(refractiveIndex*refractiveIndex) < 0;
}

/**
 * Reflect a normal on a plane given its normal vector
 */
Vec3f reflectVector(
    Vec3f const & incomingVector,
    Vec3f const & normal
) {
  return incomingVector - 2*(incomingVector * normal)*normal;
}

/**
 * Traces a ray through a transparent object.
 * Source of formulas: "Ray Tracing from the Ground Up" - ISBN 978-1-56881-272-4
 *
 * @param mesh triangle mesh representing the transparent object
 * @param origin point where the ray enters the object
 * @param transmissionVector direction the ray initially travels in the
 *   transparent object (after refraction).
 *   It can for example be computed using calculateTransmissionVector().
 * @param max_recursions maximum number of recursive raycasts to perform
 * @param refractiveIndex refractive index of the transparent object
 * @param outgoingRays output parameter which will store all rays exiting the
 *   object and the number of recursions remaining for each of them.
 */
void transmission(
    TriangleMesh const & mesh,
    Vec3f origin,
    Vec3f transmissionVector,
    std::size_t max_recursions,
    float refractiveIndex,

    std::vector<std::pair<Ray<float>, std::size_t>> & outgoingRays
) {
  Vec3f hitPoint;
  Vec3f hitNormal;

  const Ray<float> ray {origin + rayOffset * transmissionVector, transmissionVector};

  const bool hitSomething = calculateHitInMedium(
      mesh, ray,
      hitPoint, hitNormal
  );

  if (hitSomething) {
    const float cosThetaI = (-1.0f) * transmissionVector * hitNormal;
    // Comment out the "if" to disable handling of total internal reflection
    if (!isTotalInternalReflection(cosThetaI, refractiveIndex)) {
      const Vec3f outgoingVector = calculateTransmissionVector(
          1/refractiveIndex,
          ray,
          hitNormal
      );

      outgoingRays.emplace_back(
          Ray<float> {hitPoint + rayOffset * outgoingVector, outgoingVector},
          max_recursions
      );
    }

    if (max_recursions > 0) {
      const Vec3f reflectedVector = reflectVector(
          ray.d, hitNormal
        );

      transmission(
          mesh,
          hitPoint,
          reflectedVector,
          max_recursions - 1,
          refractiveIndex,
          outgoingRays
      );
    }
  }

  else { // this should never happen, we assume closed objects
    std::cout << "Transmission ray inside object does not hit anything. This should be impossible.\n";
  }
}

/**
 * Performs (recursive) raycasting and returns the intensity of the first hit
 * point.
 */
Vec3f raycast(Ray<float> const & ray, unsigned max_recursions, unsigned & hits) {
  float t = 1000.0f;              // ray parameter hit point, initialized with max view length
  float u,v;                      // barycentric coordinates (w = 1-u-v)

  // intersection test
  int hitMeshIdx; // will be set to the index of the mesh hit by the ray
  unsigned int hitTri; // will be set to the index of the hit triangle in the mesh
  if ((hitMeshIdx = intersectRayObjectsEarliest(ray,t,u,v,hitTri)) != -1) {
    auto const & hitObject = objects[hitMeshIdx];
    auto const & hitMesh = meshes[hitMeshIdx];
    auto const & hitTriangle = hitMesh.getTriangles()[hitTri];
    auto const & hitP0 = hitMesh.getVertices()[hitTriangle[0]];
    auto const & hitP1 = hitMesh.getVertices()[hitTriangle[1]];
    auto const & hitP2 = hitMesh.getVertices()[hitTriangle[2]];
    auto const & hitN0 = hitMesh.getNormals()[hitTriangle[0]];
    auto const & hitN1 = hitMesh.getNormals()[hitTriangle[1]];
    auto const & hitN2 = hitMesh.getNormals()[hitTriangle[2]];

    const Vec3f ambientIntensity {
      lightIntensityAmbient[0] * hitObject.matAmbient[0],
      lightIntensityAmbient[1] * hitObject.matAmbient[1],
      lightIntensityAmbient[2] * hitObject.matAmbient[2],
    };

    const auto w = 1-u-v;
    const Vec3f hitPointPos = w*hitP0 + u*hitP1 + v*hitP2;

    Vec3f L = lightPos - hitPointPos;
      const auto lightDistance = L.length();
      L.normalize();
    const Vec3f N = (w*hitN0 + u*hitN1 + v*hitN2).normalized();

    Vec3f recursiveIntensity = Vec3f {0.0f, 0.0f, 0.0f};
    Vec3f opacityIntensity = Vec3f {0.0f, 0.0f, 0.0f};
    if (max_recursions > 0) {
      // Reflection
      if (hitObject.matReflectiveness[0] != 0.0f || hitObject.matReflectiveness[1] != 0.0f || hitObject.matReflectiveness[2] != 0.0f) {
        const Vec3f reflectionVector = reflectVector(ray.d, N);
        recursiveIntensity = raycast(
            Ray<float> {hitPointPos + rayOffset * reflectionVector, reflectionVector},
            max_recursions - 1,
            hits
        );
      }

      // Transparency
      if (hitObject.matOpacity[0] != 1.0f || hitObject.matOpacity[1] != 1.0f || hitObject.matOpacity[2] != 1.0f) {
        const Vec3f transmittedVector = calculateTransmissionVector(
            hitObject.matRefractiveIndex, ray, N
        );

        // List of outgoing vectors with remaining recursions
        std::vector<std::pair<Ray<float>, std::size_t>> outgoingRays;
        transmission(
            hitMesh,
            hitPointPos,
            transmittedVector,
            max_recursions - 1,
            hitObject.matRefractiveIndex,
            outgoingRays
        );

        if (outgoingRays.empty()) {
          std::cout << "There is no outgoing ray from a transparent object, too low recursion depth?\n";
        }

        // perform raycast for every outgoing ray from the transparent object
        // and add the result to the `opacityIntensity`
        for (auto [ray, remaining_recursions] : outgoingRays) {
          if (remaining_recursions > 0) {
            opacityIntensity += raycast(
                ray,
                remaining_recursions - 1,
                hits
            );
          }
        }

        // Uncomment this and comment out the above, if only the first outgoing
        // ray from a transparent medium should be used.
        // In that case you should also disable total internal reflection in
        // the function transmission()
        //if (!outgoingRays.empty()) {
        //  auto [ray, remaining_recursions] = outgoingRays.front();

        //  if (remaining_recursions > 0) {
        //    opacityIntensity = raycast(
        //        ray,
        //        remaining_recursions - 1,
        //        hits
        //    );
        //  }
        //}
      }

    }

    Vec3f rgb = ambientIntensity
      + Vec3f{
          recursiveIntensity[0] * hitObject.matReflectiveness[0],
          recursiveIntensity[1] * hitObject.matReflectiveness[1],
          recursiveIntensity[2] * hitObject.matReflectiveness[2],
        }
      + Vec3f {
          opacityIntensity[0] * hitObject.matOpacity[0],
          opacityIntensity[1] * hitObject.matOpacity[1],
          opacityIntensity[2] * hitObject.matOpacity[2]
        };

    const Ray<float> lightRay {hitPointPos, L};
    const bool inShadow = !hitsLight( lightRay, lightDistance );

    if (!inShadow) {
      const Vec3f V = (-1.0f) * ray.d;
      const Vec3f H = (L+V).normalized();

      rgb += Vec3f {
          calcLight(hitObject, L, N, H, 0), // red
          calcLight(hitObject, L, N, H, 1), // green
          calcLight(hitObject, L, N, H, 2) // blue
      };
    }

    hits++;

    return rgb;
  }

  else {
    return Vec3f {
      0.f, 0.f, 0.f
    };
  }
}

void raytrace() {
  // initialization
  GLdouble MV[16]; // will store modelview matrix
  GLdouble PR[16]; // will store projection matrix
  GLint VP[4]; // will store coordinates of viewport

  glGetDoublev(GL_MODELVIEW_MATRIX, MV);
  glGetDoublev(GL_PROJECTION_MATRIX, PR);
  glGetIntegerv(GL_VIEWPORT, VP);

  // stores RGB values for every pixel in the viewport
  vector<Vec3f> pictureRGB(VP[2]*VP[3]);

  // execution statistics
  intersectionTests = 0;
  unsigned int hits = 0;
  const clock_t clockStart = clock();
  std::cout << "   10   20   30   40   50   60   70   80   90  100" << endl;
  std::cout << "====|====|====|====|====|====|====|====|====|====|" << endl;


  // FIXME: Clear debugging markers, can be removed later
  hitSpheres.clear();
  castSpheres.clear();
  rays.clear();
  rays2.clear();
  markers.clear();

  // iterate over all pixels
  //unsigned int pixelCounter = 0;
  #pragma omp parallel for schedule(dynamic)
  for (int y = VP[1]; y < VP[1] + VP[3]; ++y) {
    for (int x = VP[0]; x < VP[0] + VP[2]; ++x) {
      // FIXME: Debugging variable, remove later
      //isMiddle = y == VP[1] + (VP[3] - VP[1]) / 2 && x == VP[0] + (VP[2] - VP[0]) / 2;

      // get pixel index for addressing pictureRGB array
      const int pixel = (y-VP[1])*(VP[2]-VP[0]) + x - VP[0];

      // points on near and far plane corresponding to the current pixel
      GLdouble end[3];
      GLdouble eye[3];
      // convert pixel coordinate to two points on near and far plane in world coordinates
      gluUnProject(x,y,-1, MV, PR, VP, &eye[0], &eye[1], &eye[2]);
      gluUnProject(x,y, 1, MV, PR, VP, &end[0], &end[1], &end[2]);

      // create primary ray from viewport pixel to far plane
      float endF[3] {
        (float) end[0],
        (float) end[1],
        (float) end[2]
      };
      float eyeF[3] {
        (float)eye[0],
        (float)eye[1],
        (float)eye[2]
      };
      Ray<float> ray(&eyeF[0], &endF[0]);

      pictureRGB[pixel] = raycast(ray, max_recursion_depth, hits);

      // cout "." every 1/50 of all pixels
      // disable printing
      //#pragma omp flush (pixelCounter)
      //pixelCounter++;
      //#pragma omp flush (pixelCounter)
      //if (pixelCounter % (VP[2]*VP[3]/50) == 0) {
      //  cout << ".";
      //}
    }
  }

  const clock_t clockEnd = clock();
  const int msPassed = (clockEnd - clockStart)*1000/CLOCKS_PER_SEC;
  cout << endl << "finished. tests: " << intersectionTests << ", hits: " << hits << ", ms: " << msPassed << endl;

  // generate openGL texture
  const float mul = 255.0f; // multiply rgb values within [0,1] by 255
  cout << "normalizing picture with multiplicator " << mul << endl << endl;
  vector<GLuint> picture(VP[2]*VP[3]);
  for (int y = VP[1]; y < VP[1] + VP[3]; y++) {
    for (int x = VP[0]; x < VP[0] + VP[2]; x++) {
      // get pixel index for addressing pictureRGB array
      const int pixel = (y-VP[1])*(VP[2]-VP[0]) + x - VP[0];

      // cap R G B within 0 and 255
      const int R = max(0, min((int)(mul*pictureRGB[pixel][0]), 255));
      const int G = max(0, min((int)(mul*pictureRGB[pixel][1]), 255));
      const int B = max(0, min((int)(mul*pictureRGB[pixel][2]), 255));

      // enter R G B values into GLuint array
      picture[pixel] = R | (G<<8) | (B<<16);
    }
  }

  // generate texture
  glGenTextures(1, &raytracedTextureID);
  glBindTexture(GL_TEXTURE_2D, raytracedTextureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VP[2], VP[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, &picture[0]);
}

/**
 * @param ray ray from eye to far plane
 * @param t position on the ray, initially set to the maximum view distance.
 *   After execution of this function it will be set to the position where the
 *   first triangle has been hit (depending on the sorting of triangles in `meshes`).
 * @param u will be set to a barycentric coordinate of the position where the
 *   triangle has been hit
 * @param v output for other barycentric coordinate
 * @param hitTri holds index of hit triangle in TriangleMesh after execution
 *
 * @returns index in `meshes` array to mark the mesh that has been hit by the ray
 *   or -1 if no mesh has been hit.
 */
int intersectRayObjectsEarliest(const Ray<float> &ray, float &t, float &u, float &v, unsigned int &hitTri) {
  // We will cache the results for the closest hit found so far in these variables
  auto closestT = std::numeric_limits<float>::max(); // set it initially to the greatest value possible, so that any possible hit to be found is smaller
  float closestU = 0;
  float closestV = 0;
  unsigned int closestHitTriIdx = 0;
  unsigned int closestMeshIdx = 0;

  // indicates, if we hit anything
  bool hitSomething = false;

  // iterate over all meshes
  for (unsigned int i = 0; i < meshes.size(); i++) {
    // optional: check ray versus bounding box first (t must have been initialized!)
    //if (!rayAABBIntersect(ray, meshes[i].boundingBoxMin, meshes[i].boundingBoxMax, 0.0f, t)) {
    //  continue;
    //}

    // get triangle information
    vector<Vec3f> const & vertices   = meshes[i].getVertices();
    vector<Vec3ui> const & triangles = meshes[i].getTriangles();

    // brute force: iterate over all triangles of the mesh
    for (unsigned int j = 0; j < triangles.size(); j++) {
      Vec3f const & p0 = vertices[triangles[j][0]];
      Vec3f const & p1 = vertices[triangles[j][1]];
      Vec3f const & p2 = vertices[triangles[j][2]];

      // check if triangle is hit by ray
      const bool hit = ray.triangleIntersect(&p0.x, &p1.x, &p2.x, u, v, t);
      ++intersectionTests;

      // if so, we can record which triangle was hit and return
      if (hit && t > 0.0f) {
        if (t < closestT) {
          closestT = t;

          closestU = u;
          closestV = v;
          closestHitTriIdx = j;
          closestMeshIdx = i;

          hitSomething = true;
        }
      }
    }
  }

  if (hitSomething) {
    t = closestT;
    u = closestU;
    v = closestV;
    hitTri = closestHitTriIdx;

    return closestMeshIdx;
  }

  else {
    // return -1, if no triangle was hit by the ray
    return -1;
  }
}

// Smits� method: Brian Smits. Efficient bounding box intersection. Ray tracing news, 15(1), 2002.
bool rayAABBIntersect(const Ray<float> &r, const Vec3f& vmin, const Vec3f& vmax, float t0, float t1) {
  // Original implementation seems flawed..
  //
  //float tmin, tmax, tymin, tymax, tzmin, tzmax;
  //if (r.d.x >= 0) {
  //  tmin = (vmin.x - r.o.x) / r.d.x;
  //  tmax = (vmax.x - r.o.x) / r.d.x;
  //}
  //else {
  //  tmin = (vmax.x - r.o.x) / r.d.x;
  //  tmax = (vmin.x - r.o.x) / r.d.x;
  //}
  //if (r.d.y >= 0) {
  //  tymin = (vmin.y - r.o.y) / r.d.y;
  //  tymax = (vmax.y - r.o.y) / r.d.y;
  //}
  //else {
  //  tymin = (vmax.y - r.o.y) / r.d.y;
  //  tymax = (vmin.y - r.o.y) / r.d.y;
  //}
  //if ( (tmin > tymax) || (tymin > tmax) )
  //  return false;

  //if (tymin > tmin)
  //  tmin = tymin;
  //if (tymax < tmax)
  //  tmax = tymax;

  //if (r.d.z >= 0) {
  //  tzmin = (vmin.z - r.o.z) / r.d.z;
  //  tzmax = (vmax.z - r.o.z) / r.d.z;
  //}
  //else {
  //  tzmin = (vmax.z - r.o.z) / r.d.z;
  //  tzmax = (vmin.z - r.o.z) / r.d.z;
  //}

  //if ( (tmin > tzmax) || (tzmin > tmax) )
  //  return false;

  //if (tzmin > tmin)
  //  tmin = tzmin;
  //if (tzmax < tmax)
  //  tmax = tzmax;

  //return ( (tmin < t1) && (tmax > t0) );

  // Alternative Implementation copied from "Ray Tracing from the Ground Up" p. 357
  const float ox = r.o.x;
  const float oy = r.o.y;
  const float oz = r.o.z;

  const float dx = r.d.x;
  const float dy = r.d.y;
  const float dz = r.d.z;

  const float a = 1.0 / dx;

  const float x0 = vmin.x;
  const float x1 = vmax.x;

  const float y0 = vmin.y;
  const float y1 = vmax.y;

  const float z0 = vmin.z;
  const float z1 = vmax.z;

  float tx_min, ty_min, tz_min;
  float tx_max, ty_max, tz_max;

  if (a >= 0) {
    tx_min = (x0 - ox) * a;
    tx_max = (x1 - ox) * a;
  }
  else {
    tx_min = (x1 - ox) * a;
    tx_max = (x0 - ox) * a;
  }

  const float b = 1.0/dy;
  if (b >= 0) {
    ty_min = (y0 - oy) * b;
    ty_max = (y1 - oy) * b;
  }

  else {
    ty_min = (y1 - oy) * b;
    ty_max = (y0 - oy) * b;
  }

  const float c = 1.0/dz;
  if (c >= 0) {
    tz_min = (z0 - oz) * c;
    tz_max = (z1 - oz) * c;
  }
  else {
    tz_min = (z1 - oz) * c;
    tz_max = (z0 - oz) * c;
  }

  float t0_sub, t1_sub;

  if (tx_min > ty_min)
    t0_sub = tx_min;
  else
    t0_sub = ty_min;

  if (tz_min > t0_sub)
    t0_sub = tz_min;

  if (tx_max < ty_max)
    t1_sub = tx_max;
  else
    t1_sub = ty_max;

  if (tz_max < t1_sub)
    t1_sub = tz_max;

  return t0_sub < t1_sub && t1_sub > 0.00001;
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(const unsigned char key, const int x, const int y) {
  keyDown[key] = true;
	switch (key) {
    case 27:
      exit(0);
      break;
    case '1':
      glShadeModel(GL_FLAT);
      glutPostRedisplay();
      break;
    case '2':
      glShadeModel(GL_SMOOTH);
      glutPostRedisplay();
      break;
    case 'M' :
    case 'm' :
      drawMode = (drawMode + 1) % 3;
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].setDrawMode(drawMode);
      glutPostRedisplay();
      break;
    case 'h' :
    case 'H' :
      coutHelp();
      break;
    // reset view
    case 'r' :
    case 'R' :
      setDefaults();
      glutPostRedisplay();
      break;
    case 'v':
    case 'V':
      showMarks = !showMarks;
      glutPostRedisplay();
      break;
    // light movement
    case 'l' :
    case 'L' :
	    moveLight = !moveLight;
      break;
    case 'p' :
    case 'P' :
      lightPos = cameraPos;
      glutPostRedisplay();
      break;
    // raytrace
    case 'x' :
    case 'X' :
      raytrace();
      glutPostRedisplay();
      break;
    case 'y' :
    case 'Y' :
      glDeleteTextures(1,&raytracedTextureID);
      raytracedTextureID = 0;
      glutPostRedisplay();
      break;
    // color array
    case 'c' :
    case 'C' :
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithColorArray();
      glutPostRedisplay();
      break;
    // texture
    case 't' :
    case 'T' :
      for (unsigned int i = 0; i < meshes.size(); i++) {
        if (objects[i].textureID.has_value()) {
          meshes[i].toggleWithTexture();
        }
      }
      glutPostRedisplay();
      break;
    // bounding box
    case 'b' :
    case 'B' :
      for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithBB();
      glutPostRedisplay();
      break;
    // normal
    case 'n' :
    case 'N' :
     for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].toggleWithNormals();
      glutPostRedisplay();
      break;
    // movement speed up
    case '+' :
      movementSpeed *= 2.0f;
      break;
    // movement speed down
    case '>' :
      ++max_recursion_depth;
      std::cout << "Max recursion depth is now " << max_recursion_depth << std::endl;
      break;
    case '<' :
      if (max_recursion_depth > 0) {
        --max_recursion_depth;
      }
      std::cout << "Max recursion depth is now " << max_recursion_depth << std::endl;
      break;
	}
}

void keyReleased(const unsigned char key, const int x, const int y) {
  keyDown[key] = false;
}

void mousePressed(const int button, const int state, const int x, const int y) {
  mouseButton = button;
  mouseX = x;
  mouseY = y;
}

void mouseMoved(int x, int y) {
  // update angle with relative movement
  angleX = fmod(angleX + (x-mouseX)*mouseSensitivy,360.0f);
  angleY -= (y-mouseY)*mouseSensitivy;
  angleY = max(-70.0f,min(angleY,70.0f));
  // calculate camera direction (vector length 1)
  cameraDir.x =  sin(angleX*M_RadToDeg) * cos(angleY*M_RadToDeg);
  cameraDir.z = -cos(angleX*M_RadToDeg) * cos(angleY*M_RadToDeg);
  cameraDir.y = max(0.0f,min(sqrtf(1.0f - cameraDir.x*cameraDir.x -  cameraDir.z* cameraDir.z),1.0f));
  if (angleY < 0) cameraDir.y = -cameraDir.y;
  // update mouse for next relative movement
  mouseX = x;
  mouseY = y;
  glutPostRedisplay();
}

// ===============
// === VARIOUS ===
// ===============

void coutHelp() {
  cout << endl;
  cout << "====== KEY BINDINGS ======" << endl;
  cout << "ESC: exit" << endl;
  cout << "H: show this (h)elp file" << endl;
  cout << "R: (r)eset to default values" << endl;
  cout << "L: toggle (l)ight movement" << endl;
  cout << "P: move the light to current (p)osition" << endl;
  cout << endl;
  cout << "A,W,S,D,Q,E: first person movement" << endl;
  cout << "+,-: movement speed up and down" << endl;
  cout << endl;
  cout << "1: GL Shader FLAT" << endl;
  cout << "2: GL Shader SMOOTH" << endl;
  cout << endl;
  cout << "M: switch draw (m)ode. 0: immediate, 1: array, 2: VBO" << endl;
  cout << "C: toggle use of (c)olor array" << endl;
  cout << "T: toggle use of (t)exture coordinates" << endl;
  cout << "B: toggle drawing (b)ounding box" << endl;
  cout << "N: toggle drawing (n)ormals" << endl;
  cout << endl;
  cout << "X: raytrace scene" << endl;
  cout << "Y: clear and switch back to openGL rendering" << endl;
  cout << endl;
  cout << ">: Increase max recursion depth" << endl;
  cout << "<: Decrease max recursion depth" << endl;
  cout << "==========================" << endl;
  cout << endl;
}
