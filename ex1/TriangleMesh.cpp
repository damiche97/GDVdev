// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes            //
// ========================================================================== //

#include <iostream>
#include <fstream>
#include <float.h>
#include <GL/glut.h>
#include "TriangleMesh.h"

#include <cassert>

void TriangleMesh::calculateNormals() {
  normals.resize(vertices.size());
  // calculate normals for each triangle
  for (Triangle const & t : triangles) {
	  // retrieve immutable references to each vertex of the triangle
	  Vec3f const & p1 = vertices[t[0]];
	  Vec3f const & p2 = vertices[t[1]];
	  Vec3f const & p3 = vertices[t[2]];

	  const Vec3f edge1 = p1 - p2;
	  const Vec3f edge2 = p1 - p3;

	  const Vec3f normal = edge1 ^ edge2;
	  normals[t[0]] += normal;
	  normals[t[1]] += normal;
	  normals[t[2]] += normal;
  }

  // normalize normals
  for (Normals::iterator nit = normals.begin(); nit!=normals.end(); ++nit) {
     //the normalize() function returns a boolean which can be used if you want to check for erroneous normals
	 (*nit).normalize();
   }

  // Die Eckpunkt-Normalen sind durch diese Methode nach Dreiecksfläche gewichtet. Warum?
  /*
     For vertices which are shared between triangles, a common normal has to be
     computed to achieve more smooth shading.
     The normals of bigger triangles should have more weight in this calculation,
     since they are more visible and contribute more to the shape of an object.
  */
}

void TriangleMesh::calculateNormalsWithAngle()
{
	normals.resize(vertices.size());
	for (Triangle const & t : triangles) {
		//p1
		Vec3f const & p1 = vertices[t[0]];
		Vec3f const & p2 = vertices[t[1]];
		Vec3f const & p3 = vertices[t[2]];

		Vec3f edge1 = p1 - p2;
		Vec3f edge2 = p1 - p3;
		Vec3f edge3 = p2 - p3;

		edge1.normalize();
		edge2.normalize();
		edge3.normalize();

		const float angle1 = acos(edge1 * edge2);
		const float angle2 = acos(edge1 * edge3 * -1);
		const float angle3 = acos(edge2 * edge3);

		Vec3f normal = edge1 ^ edge2;
		normals[t[0]] += angle1 *normal;
		normals[t[1]] += angle2* normal;
		normals[t[2]] += angle3* normal;
	}

	// normalize normals
	for (Normals::iterator nit = normals.begin(); nit != normals.end(); ++nit) {
		//the normalize() function returns a boolean which can be used if you want to check for erroneous normals
		(*nit).normalize();
	}

  // Purpose of this altered normal calculation:
  // Triangle faces should not contribute largely to a normal calculation, if
  // only a very slim corner is part of the corresponding vertex.
  // Thus, normal calculation should also be weighted by the size of angles.
}

void TriangleMesh::setUseAngleNormals(bool useAngleNormals) {
  this->useAngleNormals = useAngleNormals;
}

// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh() {
  clear();
}

TriangleMesh::~TriangleMesh() {
  clear();
}

void TriangleMesh::clear() {
  // clear mesh data
  vertices.clear();
  triangles.clear();
  normals.clear();
}

// ================
// === RAW DATA ===
// ================

vector<Vec3f>& TriangleMesh::getPoints() {
  return vertices;
}
vector<Vec3i>& TriangleMesh::getTriangles() {
	return triangles;
}

vector<Vec3f>& TriangleMesh::getNormals() {
  return normals;
}

void TriangleMesh::flipNormals() {
  for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) {
    (*it) *= -1.0;
  }
}

// =================
// === LOAD MESH ===
// =================

float degToRadian(float deg) {
  return (deg * M_PI) / 180.0f;
}

void TriangleMesh::loadLSA(const char* filename) {  
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadLSA: can not open " << filename << endl;
    return;
  }
  char s[256];
  in >> s;
  // first word: LSA
  if (!(s[0] == 'L' && s[1] == 'S' && s[2] == 'A')){
      cout << "Can't read LSA file!" << endl;
  }
  // get number of vertices nv, faces nf, edges ne and baseline distance
  std::size_t nv, nf, ne;
  float baseline;
  in >> nv;
  in >> nf;
  in >> ne;
  in >> baseline;
  if (nv <= 0 || nf <= 0) return;
  // clear any existing mesh
  clear();

  // read vertices  
  vertices.reserve(nv);
  // read alpha, beta, gamma for each vertex and calculate verticex coordinates
  for(std::size_t i = 0; i < nv; ++i){
    float alpha_deg; in >> alpha_deg;
    float beta_deg; in >> beta_deg;
    float gamma_deg; in >> gamma_deg;

    // since the file stores the angles as degrees, we have to convert them to
    // radians first
    const float alpha = degToRadian(alpha_deg);
    const float beta = degToRadian(beta_deg);
    const float gamma = degToRadian(gamma_deg);

    // Now we can compute the (x,y,z) coordinates using the formulas from the
    // theory exercise
    const float z = -baseline/(tan(alpha) + tan(beta));
    const float x = -z * tan(beta);
    const float y = -z * tan(gamma);

    vertices.push_back(Vertex{x, y, z});
  }

  // read triangles
  triangles.resize(nf);
  // read all triangles from the file
  for(std::size_t i = 0; i < triangles.size(); ++i){
    std::size_t Anzahl; in >> Anzahl;

    in >> triangles[i][0];
    in >> triangles[i][1];
    in >> triangles[i][2];
  }
  // calculate normals
  if (useAngleNormals) {
    calculateNormalsWithAngle();
  }

  else {
    calculateNormals();
  }
}

void TriangleMesh::loadOFF(const char* filename) {
  // open file for reading
  std::ifstream in(filename);
  if (!in.is_open()) {
    cout << "loadOFF: can not find " << filename << endl;
    return;
  }

  // buffer for reading strings from file
  char s[256];
  in >> s;
  // first word: OFF
  if (!(s[0] == 'O' && s[1] == 'F' && s[2] == 'F')) return;
  // get number of vertices nv, faces nf, edges ne and baseline distance
  int nv, nf, ne;
  in >> nv;
  in >> nf;
  in >> ne;
  if (nv <= 0 || nf <= 0) return; // immediately abort if there are no vertices/faces
  // clear any existing mesh
  clear();
  // read vertices  
  vertices.reserve(nv); // reserve memory to store nv vertices
  for (std::size_t i = 0; i < nv; ++i) { // for the number of vertices stored in the file...
    float x; in >> x; // read coordinates
    float y; in >> y;
    float z; in >> z;

    vertices.push_back(Vertex{x, y, z}); // store them in vector
  }

  // read triangles
  triangles.reserve(nf); // reserve memory for nf faces
  for (std::size_t i = 0; i < nf; ++i) { // for every face stored in the file...
    // read the number of vertices the face is composed off
    std::size_t numVertices; in >> numVertices;
    // we only support triangles, therefore an error shall be raised, if the number of vertices of a face is != 3
    assert(numVertices == 3);

    // read the index of the 3 vertices from which a triangle is built
    int vert1Idx; in >> vert1Idx;
    int vert2Idx; in >> vert2Idx;
    int vert3Idx; in >> vert3Idx;

    // store the vertex indices
    triangles.push_back(Triangle{vert1Idx, vert2Idx, vert3Idx});
  }

  // Assert, that there are no edges stored in the OFF file since they are
  // implied by the face definitions
  assert(ne == 0);

  // calculate normals
  if (useAngleNormals) { // calculate normals with angles, if the corresponding flag is set
    calculateNormalsWithAngle();
  }

  else { // use standard calculation otherwise
    calculateNormals();
  }
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::draw() {
  if (triangles.size() == 0) return;
  // TODO: draw triangles with immediate mode
  for (Triangle t : triangles) {
	  glBegin(GL_TRIANGLES);
	  glNormal3f(normals[t[0]][0], normals[t[0]][1], normals[t[0]][2]);
	  glVertex3f(vertices[t[0]][0], vertices[t[0]][1], vertices[t[0]][2]);

	  glNormal3f(normals[t[1]][0], normals[t[1]][1], normals[t[1]][2]);
	  glVertex3f(vertices[t[1]][0], vertices[t[1]][1], vertices[t[1]][2]);

	  glNormal3f(normals[t[2]][0], normals[t[2]][1], normals[t[2]][2]);
	  glVertex3f(vertices[t[2]][0], vertices[t[2]][1], vertices[t[2]][2]);
	  glEnd();
  }
}

void TriangleMesh::add(Vec3f p1, Vec3f p2, Vec3f p3) {
  auto initialSize = vertices.size();

  vertices.reserve(3);
  vertices.push_back(std::move(p1));
  vertices.push_back(std::move(p2));
  vertices.push_back(std::move(p3));

  triangles.emplace_back(initialSize, initialSize + 1, initialSize + 2);
}
