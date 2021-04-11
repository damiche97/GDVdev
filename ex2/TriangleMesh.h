// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes            //
//   * readOFF                                                                //
//   * draw                                                                   //
//   * transformations                                                        //
// ========================================================================== //

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include "Vec3.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <algorithm>

#include "Matrix.h"

#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

using namespace std;

enum class MeshType {
  Static,
  Dynamic // VBO rendering will be set up to support continuous changes to rendered vertices
};

class TriangleMesh  {

private:
  MeshType meshType;

  // typedefs for data
  typedef Vec3ui Triangle;
  typedef Vec3f Normal;
  typedef Vec3f Vertex;
  typedef vector<Triangle> Triangles;
  typedef vector<Normal> Normals;
  typedef vector<Vertex> Vertices;  

  // data of TriangleMesh
  Vertices vertices;
  Normals normals;
  Triangles triangles;

  // VBO ids for vertices, normals and faces
  bool VBOsupported;
  unsigned int VBOv, VBOn, VBOf;

  // bounding box data
  Vec3f boundingBoxMin;
  Vec3f boundingBoxMax;
  Vec3f boundingBoxMid;
  Vec3f boundingBoxSize;

  // =========================
  // === PRIVATE FUNCTIONS ===
  // =========================

  // calculate normals
  void calculateNormals();

  // create VBOs for vertices, faces and normals
  void createAllVBOs();

  // clean up VBO data (delete from gpu memory)
  void cleanupVBO();  

  // update VBOs after matrix multiplication
  void updateVBOs();  

public:

  // ===============================
  // === CONSTRUCTOR, DESTRUCTOR ===
  // ===============================

  TriangleMesh(MeshType meshType);
  ~TriangleMesh();

  // clears all data, sets defaults
  void clear();

  /**
   * Applies transformation matrix to all vertices of mesh.
   * Updates normals and VBO buffers accordingly.
   */
  void applyMatrix(Matrix const & m);

  // ================
  // === RAW DATA ===
  // ================

  // get raw data references
  vector<Vec3f>& getVertices() { return vertices; }
  vector<Vec3ui>& getTriangles() { return triangles; }
  vector<Vec3f>& getNormals() { return normals; }

  // get size of all elements
  unsigned int getNumVertices() { return vertices.size(); }
  unsigned int getNumNormals() { return normals.size(); }
  unsigned int getNumTriangles() { return triangles.size(); }

  // flip all normals
  void flipNormals();

  // translates vertices so that the bounding box center is at newBBmid
  void translateToCenter(const Vec3f& newBBmid);

  // scales vertices so that the largest bounding box size has length newLength
  void scaleToLength(const float newLength);

  // =================
  // === LOAD MESH ===
  // =================

  // read from an OFF file. also calculates normals if not given in the file.
  void loadOFF(const char* filename);

  // read from an OFF file. also calculates normals if not given in the file.
  // translates and scales vertices with bounding box center at BBmid and largest side BBlength
  void loadOFF(const char* filename, const Vec3f& BBmid, const float BBlength);

  // ==============
  // === RENDER ===
  // ==============
  
  // draw mesh with immediate mode
  void drawImmediate();

  // draw mesh with vertex array
  void drawArray(); 

  // draw VBO
  void drawVBO();

};


#endif

