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

#include <stdio.h>        // cout
#include <iostream>       // cout
#include <fstream>        // read file
#include <math.h>         // fmod
#include "main.h"         // this header
#include <algorithm>
#include <string>
#include <memory>
#include <chrono>
#include <optional>
#include <functional>
#include <tuple>

#include "Timer.h"
#include "GridTask.h"
#include "SceneTask.h"

// ==============
// === BASICS ===
// ==============

// If set to true, the time necessary to render a frame will be recorded.
// This flag can be switched by pressing 'r'
bool measureRenderTime = false;
Timer timer; // Time measuring object

// This integer indicates, which scene we are currently rendering, see below.
// It will be adjusted, whenever the user presses 't' to switch to the
// next scene.
unsigned int taskSelection = 0;

// This array contains functions to construct the implemented scene objects:
//  0: The dolphin grid scene
//  1: The dynamic scene
//
// It will be used to switch between the scenes at runtime
std::tuple<
  const char *,                           // task name
  std::function<std::unique_ptr<Task> ()> // factory function to create object
> taskFactories[] = {
  { "Grid Scene", []() { return std::make_unique<GridTask>(); } },
  { "Dynamic Scene", []() { return std::make_unique<SceneTask>(); } }
};

// Pointer to the task to be rendered. Will be set by setTaskFromSelection below
std::unique_ptr<Task> task;

// Variable to track the progress of time for a scene.
// We are using optionals so that we can simply reset time by assigning
// std::nullopt to this variable.
std::optional<
  std::chrono::time_point< // We are using the C++ chrono library, since it provides nice type safe tools to deal with time
    std::chrono::system_clock // we are measuring time with the standard system clock
  >
> last_render_time = std::nullopt;

/**
 * This function intializes the `task` object depending on the current value
 * of the `taskSelection` variable, which is updated when the user presses 't'.
 */
const char * setTaskFromSelection() {
  // fetch the next scene from the array `taskFactories`
  auto & selection = taskFactories[taskSelection];

  // The array provides the scene's name and a function to create it
  auto & name = std::get<0>(selection);
  auto & factory = std::get<1>(selection);

  // create the scene object.
  // Since we are using smart pointers (std::unique_ptr), this will also delete
  // the previous scene.
  task = factory();

  // Load data of scene and reset time measurement variables
  task->load();
  last_render_time = std::nullopt;
  timer.reset();
  timer.resetAverage();

  // return the name
  return name;
}

int main(int argc, char** argv) {
  // initialize openGL window
  glutInit(&argc, argv);
  glutInitWindowPosition(300,200);
  glutInitWindowSize(600,400);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Framework VBO and Shader");
  // link functions to certain openGL events
  glutDisplayFunc(renderScene);
  glutReshapeFunc(reshape);
  glutMouseFunc(mousePressed);
  glutMotionFunc(mouseMoved);
  glutKeyboardFunc(keyPressed);
  // further initializations
  setDefaults();
  initialize();
  // load shaders
  GLuint programID = 0;
  GLuint programID2 = 0;
  GLuint programID3 = 0;
  GLuint programID4 = 0;
  programID = readShaders("Shader/FlatGreyShader.vert", "Shader/FlatGreyShader.frag");
  programID2 = readShaders("Shader/Toon_Shader.vert", "Shader/Toon_Shader.frag");
  programID3 = readShaders("Shader/Toon_Shader2.vert", "Shader/Toon_Shader2.frag");
  programID4 = readShaders("Shader/Toon_Shader3.vert", "Shader/Toon_Shader3.frag");

  if (programID != 0) programIDs.push_back(programID);
  if (programID2 != 0) programIDs.push_back(programID2);
  if (programID3 != 0) programIDs.push_back(programID3);
  if (programID4 != 0) programIDs.push_back(programID4);
  cout << programIDs.size() << " shaders loaded. Use keys 3 to " << programIDs.size() + 2 << "." << endl;
  // load task
  setTaskFromSelection();
  // activate main loop
  glutTimerFunc(20, processTimedEvent, 0);
  glutIdleFunc(animate);
  glutMainLoop();
  return 0;
}

void animate() {
  std::chrono::system_clock::duration time_delta;
  const auto now = std::chrono::system_clock::now();
  if (last_render_time.has_value()) {
    time_delta = now - *last_render_time;
  }
  else {
    time_delta = std::chrono::system_clock::duration(0);
  }
  last_render_time.emplace(now);

  // Since we want to count updates to the VBO buffer as rendering time,
  // we also call the timer here
  if (measureRenderTime) {
    glFinish(); // force opengl to finish all previous operations first, before measuring time
    timer.start();
  }
  bool renderAgain = task->tick(
      std::chrono::duration_cast<std::chrono::milliseconds>(time_delta)
  );
  if (measureRenderTime) {
    timer.stop();
  }

  if (renderAgain) {
    glutPostRedisplay();
  }
}

void setDefaults() {
  // scene Information
  centerPos.set(0.0f, -4.0f, -8.0f);
  angleX = 0.0f;
  angleY = 0.0f;
  // light information
  lightPos.set(-30.0f, 10.0f, 0.0f);
  lightMotionSpeed = 2.0f;
  moveLight = false;
  // mouse information
  mouseX = 0;
  mouseY = 0;
  mouseButton = 0;
  mouseSensitivy = 1.0f;
  // immediate draw mode
  drawMode = 2;
  taskSelection = 0;
  // reset FPS
  fpsCout = false;
  fpsClock = clock();
  fpsCounterFrames = 0;
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
    glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv) {
      cout << "VBOs are supported!" << endl;
  }
  else {
    cout << "VBOs are NOT supported!" << endl;
  }
  // enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // normalize normals because of glScalef used
  glEnable(GL_NORMALIZE);
  // set shading model
  glShadeModel(GL_SMOOTH);
  // set lighting (white light)
  GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
  GLfloat ambientLight[] =   { 0.1f, 0.1f, 0.1f, 1.0f };
  GLfloat diffuseLight[] =   { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat specularLight[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat shininess = 128.0f;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightf(GL_LIGHT0, GL_SHININESS, shininess);
  glEnable(GL_LIGHT0);
  // enable use of glColor instead of glMaterial
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  // white shiny specular highlights
  GLfloat specularLightMaterial[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat shininessMaterial = 128.0f;
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininessMaterial);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, specularLightMaterial);

  // keybindings => cout
  coutHelp();
}

void reshape(GLint width, GLint height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (float)width / (float)height, 0.1, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void processTimedEvent(int x) {
  // Time now
  clock_t clock_this_run = clock();
  // perform light movement and trigger redisplay
  if (moveLight == true) {
    lightPos.rotY(lightMotionSpeed);
    glutPostRedisplay();
  }
  // calculate FPS
  if (fpsCout == true) {
    int msPassedFPS = clock() - fpsClock;
    // cout fps after roughly one second
    if (msPassedFPS > 1000) {
      int triangles = task->getNumTriangles();
      float fps = 1000.0f*fpsCounterFrames/msPassedFPS;
      int tps = (int)(fps*triangles);
      cout << fpsCounterFrames << " frames in " << msPassedFPS << " ms: " << fps << " FPS, " << 0.000001*tps << "M triangles/sec." << endl;
      fpsClock = clock();
      fpsCounterFrames = 0;
    }
    // redraw as soon as possible for fps calculation
    glutPostRedisplay();
    glutTimerFunc(1 , processTimedEvent, 0);
    return;
  }
  // start event again
  int msPassed = clock() - clock_this_run;
  glutTimerFunc(std::max(1,20-msPassed) , processTimedEvent, 0);
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
  if (measureRenderTime) {
    timer.start();
  }
  
  // fps counter
  fpsCounterFrames++;
  // clear and set camera
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  // translate to centerPos
  glTranslatef(centerPos.x, centerPos.y, centerPos.z);
  // rotate scene
  glRotatef(angleX,0.0f,1.0f,0.0f);
  glRotatef(angleY,1.0f,0.0f,0.0f);
  // draw coordinate system without lighting
  glDisable(GL_LIGHTING);
  drawCS();
    // draw sphere for light still without lighting
  drawLight();
  // draw objects
  glEnable(GL_LIGHTING);

  task->draw(
    static_cast<DrawMode>(
        abs(drawMode) % 3
    )
  );

  if (measureRenderTime) {
    glFinish(); // force opengl to finish all previous operations first, before measuring time
    timer.stop();
    const auto millisecondsForThisFrame = std::chrono::duration_cast<std::chrono::milliseconds>(
      timer.getMeasuredTime()
    ).count();
    timer.reset();

    const auto averageMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
      timer.getAverageMeasuredTime()
    ).count();

    cout << "Rendered frame in "
         << millisecondsForThisFrame
         << "ms. Average: "
         << averageMilliseconds
         << "ms."
         << endl;
  }

  // swap Buffers
  glutSwapBuffers();
}

// =================
// === CALLBACKS ===
// =================

void keyPressed(unsigned char key, int x, int y) {
	switch (key) {
    // EXC => exit
    case 27:
      exit(0);
      break;
    // OpenGL flat shading
    case '1':
      glUseProgram(0);
      glShadeModel(GL_FLAT);
      glutPostRedisplay();
      break;
    // OpenGL smooth shading
    case '2':
      glUseProgram(0);
      glShadeModel(GL_SMOOTH);
      glutPostRedisplay();
      break;
    // shader 3-9
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
        auto n = static_cast<size_t>(key) - 48;
        cout << "key: " << to_string(n) << endl;
        if (programIDs.size() > n-3)
          glUseProgram(programIDs[n-3]);
      }
      glutPostRedisplay();
      break;
    // change draw mode
    case 'M' :
    case 'm' :
      drawMode = (drawMode + 1) % 3;
      cout << "drawMode: " << drawMode;
      switch (drawMode) {
        case 1:  cout << " = array" << endl; break;
        case 2:  cout << " = VBO" << endl; break;
        default: cout << " = immediate" << endl; break;
      }
      timer.resetAverage();
      glutPostRedisplay();
      break;
    // change task to be rendered
    case 'T' :
    case 't' :
      {
        // select the next scene from the `taskFactories` array
        taskSelection = (taskSelection + 1) % (sizeof(taskFactories)/sizeof(*taskFactories));
        auto taskName = setTaskFromSelection(); // create the scene object and retrieve its name
        cout << "task: " << taskName << "; " << task->getNumTriangles() << " triangles" << endl;
        glutPostRedisplay();
        break;
      }
    // trigger measurement of drawing time
    case 'S':
    case 's':
      measureRenderTime = !measureRenderTime;
      timer.reset();
      timer.resetAverage();
      break;
    // help text
    case 'h' :
    case 'H' :
      coutHelp();
      break;
    // reset view
    case 'r' :
    case 'R' :
      {
        auto oldTaskSelection = taskSelection;
        setDefaults();
        taskSelection = oldTaskSelection; // keep task selection on reset
        setTaskFromSelection();
        timer.resetAverage();

        glutPostRedisplay();
        break;
      }
    // Light movement
    case 'l' :
    case 'L' :
	    moveLight = !moveLight;
      break;
    // toggle FPS calculation
    case 'f':
    case 'F':
      fpsCout = !fpsCout;
      break;
    default:
      task->keyPressed(key, x, y);
	}
}

void mousePressed(int button, int state, int x, int y) {
  mouseButton = button;
  mouseX = x;
  mouseY = y;
}

void mouseMoved(int x, int y) {
  // rotate
  if (mouseButton == GLUT_LEFT_BUTTON) {
    angleX = fmod(angleX + (x-mouseX)*mouseSensitivy,360.0f);
    angleY += (y-mouseY)*mouseSensitivy;
    glutPostRedisplay();
  }
  // zoom (here translation in z)
  if (mouseButton == GLUT_RIGHT_BUTTON) {
    centerPos.z -= 0.2f*(y-mouseY)*mouseSensitivy;
    glutPostRedisplay();
  }
  // translation in xy
  if (mouseButton == GLUT_MIDDLE_BUTTON) {
    centerPos.x += 0.2f*(x-mouseX)*mouseSensitivy;
    centerPos.y -= 0.2f*(y-mouseY)*mouseSensitivy;
    glutPostRedisplay();
  }
  // update mouse for next relative movement
  mouseX = x;
  mouseY = y;
}

// ===============
// === VARIOUS ===
// ===============

void coutHelp() {
  cout << endl;
  cout << "====== KEY BINDINGS ======" << endl;
  cout << "ESC: exit" << endl;
  cout << "H:   show this (H)elp file" << endl;
  cout << "R:   (R)eset view" << endl;
  cout << "L:   toggle (L)ight movement" << endl;
  cout << "F:   toggle FPS output" << endl;
  cout << "+/-: in-/decrease the gridSize for drawn objects" << endl;
  cout << "" << endl;
  cout << "1:   GL Shader FLAT" << endl;
  cout << "2:   GL Shader SMOOTH" << endl;
  cout << "3+:  Custom Shader" << endl;
  cout << "" << endl;
  cout << "M:   Switch Draw (M)ode. 0: Immediate, 1: Array, 2: VBO" << endl;
  cout << "T:   Switch task to draw" << endl;
  cout << "S:   Measure and render time necessary to display a single frame." << endl;
  cout << "==========================" << endl;
  cout << endl;
}
