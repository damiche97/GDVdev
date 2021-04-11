// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Content: Simple class for reading and rendering triangle meshes           //
//   * readOFF                                                               //
//   * draw                                                                  //
//   * transformations                                                       //
// ==========================================================================//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <float.h>
#include "TriangleMesh.h"

// =========================
// === PRIVATE FUNCTIONS ===
// =========================

void TriangleMesh::calculateNormals() {
	// sum up triangle normals in each vertex
	normals.resize(vertices.size());
	for (Triangles::iterator it = triangles.begin(); it != triangles.end(); ++it) {
		Vec3f vec1, vec2, normal;
		unsigned int id0, id1, id2;
		id0 = (*it)[0];
		id1 = (*it)[1];
		id2 = (*it)[2];
		vec1 = vertices[id1] - vertices[id0];
		vec2 = vertices[id2] - vertices[id0];
		// cross product
		normal = vec1 ^ vec2; 
		// sum up in vertices (=> weighted by area)
		normals[id0] += normal;
		normals[id1] += normal;
		normals[id2] += normal;
	}
	for (Normals::iterator nit=normals.begin(); nit!=normals.end(); ++nit){
		//the normalize() function returns a boolean which can be used if you want to check for erroneous normals
		(*nit).normalize();
	}
}

void TriangleMesh::createAllVBOs() {
	// check if VBOs are supported
	if(glGenBuffers && glBindBuffer && glBufferData && glBufferSubData &&
		glMapBuffer && glUnmapBuffer && glDeleteBuffers && glGetBufferParameteriv)
		VBOsupported = true;
	else {
		VBOsupported = false;
		cout << "VBOs are not supported!" << endl;
	}

  // create a buffer in graphics memory for vertices
	glGenBuffers(1, &VBOv);
  // tell opengl we want to work on this buffer and it contains data, not indices
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
  // copy vertices into the buffer
	glBufferData(
     GL_ARRAY_BUFFER,                   // type of buffer
     sizeof(Vertex) * vertices.size(),  // size of the buffer
     vertices.data(),                   // data source for copying
     meshType == MeshType::Dynamic ?    // how the buffer will be used?
        GL_STREAM_DRAW    // dynamic scene (frequent modifications)
      : GL_STATIC_DRAW    // static scene (no modifications)
  );

  // create a buffer in graphics memory for normals
	glGenBuffers(1, &VBOn);
  // tell opengl we want to work on this buffer and it contains data, not indices
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
  // copy normals into the buffer
	glBufferData(
      GL_ARRAY_BUFFER, sizeof(Normal) * normals.size(), normals.data(),
      meshType == MeshType::Dynamic ?    // how the buffer will be used?
         GL_STREAM_DRAW    // dynamic scene (frequent modifications)
       : GL_STATIC_DRAW    // static scene (no modifications)
  );

  // create a buffer in graphics memory for indices
	glGenBuffers(1, &VBOf);
  // tell opengl we want to work on this buffer and it contains indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);
  // copy indices into the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), 
      GL_STATIC_DRAW // We can always use the static buffer type, since indices are never changed
  );

  // Unbind buffers.
  // Otherwise these settings would interfere with the other drawing modes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleMesh::updateVBOs() {
  // tell opengl we want to work on this buffer and it contains data, not indices
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
  // copy changed vertices into the buffer
	glBufferSubData(
     GL_ARRAY_BUFFER,                   // type of buffer
     0,                                 // start replacing buffer contents at beginning
     sizeof(Vertex) * vertices.size(),  // size of the buffer
     vertices.data()                    // data source for copying
  );

  // tell opengl we want to work on this buffer and it contains data, not indices
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
  // copy normals into the buffer
	glBufferSubData(
      GL_ARRAY_BUFFER, 0, sizeof(Normal) * normals.size(), normals.data()
  );

  // We do not need to update indices, since those are not changed by matrix
  // multiplication

  // Unbind buffers.
  // Otherwise these settings would interfere with the other drawing modes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleMesh::cleanupVBO() {
	// delete VBO
	if (VBOsupported) {
		if (VBOv != 0) glDeleteBuffers(1, &VBOv);
		if (VBOn != 0) glDeleteBuffers(1, &VBOn);
		if (VBOf != 0) glDeleteBuffers(1, &VBOf);
	}
	VBOv = 0;
	VBOn = 0;
	VBOf = 0;
}


// ===============================
// === CONSTRUCTOR, DESTRUCTOR ===
// ===============================

TriangleMesh::TriangleMesh(MeshType meshType)
  : meshType(meshType)
{
	// initialize VBO IDs with 0
	VBOv = 0;
	VBOn = 0;
	VBOf = 0;
	VBOsupported = false;
	
	// clear all data
	clear();
}

TriangleMesh::~TriangleMesh() {
	// clear all data
	clear();
}

void TriangleMesh::clear() {
	// clear mesh data
	vertices.clear();
	triangles.clear();
	normals.clear();
	// clear bounding box data
	boundingBoxMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
	boundingBoxMax.set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	boundingBoxMid.clear();
	boundingBoxSize.clear();
	// clear VBO data
	cleanupVBO();
}

// ================
// === RAW DATA ===
// ================

void TriangleMesh::applyMatrix(Matrix const & m) {
  std::transform(
      std::begin(vertices),
      std::end(vertices),
      std::begin(vertices),
      [&m](Vec3f const & v) {
        return m * v;
      }
  );

	calculateNormals();

  // recalculate VBOs
  updateVBOs();
}

void TriangleMesh::flipNormals() {
	for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) (*it) *= -1.0;
}

void TriangleMesh::translateToCenter(const Vec3f& newBBmid) {
	Vec3f trans = newBBmid - boundingBoxMid;
	for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] += trans;  
	boundingBoxMin += trans;
	boundingBoxMax += trans;
	boundingBoxMid += trans;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

void TriangleMesh::scaleToLength(const float newLength) {
	float length = max(max(boundingBoxSize.x, boundingBoxSize.y), boundingBoxSize.z);
	float scale = newLength / length;
	for (unsigned int i = 0; i < vertices.size(); i++) vertices[i] *= scale;
	boundingBoxMin *= scale;
	boundingBoxMax *= scale;
	boundingBoxMid *= scale;
	boundingBoxSize *= scale;
	// data changed => delete VBOs and create new ones (not efficient but easy)
	cleanupVBO();
	createAllVBOs();
}

// =================
// === LOAD MESH ===
// =================

void TriangleMesh::loadOFF(const char* filename) {
	std::ifstream in(filename);
	if (!in.is_open()) {
		cout << "loadOFF: can not find " << filename << endl;
		return;
	}
	const int MAX = 256;
	char s[MAX];
	in >> setw(MAX) >> s;
	// differentiate between OFF (vertices only) and NOFF (vertices and normals)
	bool noff = false;
	if (s[0] == 'O' && s[1] == 'F' && s[2] == 'F')
		;
	else if (s[0] == 'N' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F')
		noff = true;
	else
		return;
	// get number of vertices nv, faces nf and edges ne
	int nv,nf,ne;
	in >> setw(MAX) >> nv;
	in >> setw(MAX) >> nf;
	in >> setw(MAX) >> ne;
	if (nv <= 0 || nf <= 0) return;
	// clear any existing mesh
	clear();
	// read vertices
	vertices.resize(nv);
	for (int i = 0; i < nv; ++i) {
		in >> setw(MAX) >> vertices[i][0];
		in >> setw(MAX) >> vertices[i][1];
		in >> setw(MAX) >> vertices[i][2];
		boundingBoxMin[0] = std::min(vertices[i][0], boundingBoxMin[0]);
		boundingBoxMin[1] = std::min(vertices[i][1], boundingBoxMin[1]);
		boundingBoxMin[2] = std::min(vertices[i][2], boundingBoxMin[2]);
		boundingBoxMax[0] = std::max(vertices[i][0], boundingBoxMax[0]);
		boundingBoxMax[1] = std::max(vertices[i][1], boundingBoxMax[1]);
		boundingBoxMax[2] = std::max(vertices[i][2], boundingBoxMax[2]);
		if (noff) {
			in >> setw(MAX) >> normals[i][0];
			in >> setw(MAX) >> normals[i][1];
			in >> setw(MAX) >> normals[i][2];
		}
	}
	boundingBoxMid = 0.5f*boundingBoxMin + 0.5f*boundingBoxMax;
	boundingBoxSize = boundingBoxMax - boundingBoxMin;
	// read triangles
	triangles.resize(nf);
	for (int i = 0; i < nf; ++i) {
		int three;
		in >> setw(MAX) >> three;
		in >> setw(MAX) >> triangles[i][0];
		in >> setw(MAX) >> triangles[i][1];
		in >> setw(MAX) >> triangles[i][2];
	}
	// close ifstream
	in.close();
	// calculate normals if not given
	if (!noff) calculateNormals();

	// createVBO
	createAllVBOs();


}

void TriangleMesh::loadOFF(const char* filename, const Vec3f& BBmid, const float BBlength) {
	loadOFF(filename);  
	translateToCenter(BBmid);
	scaleToLength(BBlength);
	
}

// ==============
// === RENDER ===
// ==============

void TriangleMesh::drawImmediate() {
	if (triangles.size() == 0) return;
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < triangles.size(); i++) {
		for (unsigned int v = 0; v < 3; v++) {
			glNormal3f( normals[triangles[i][v]].x,  normals[triangles[i][v]].y,  normals[triangles[i][v]].z);
			glVertex3f(vertices[triangles[i][v]].x, vertices[triangles[i][v]].y, vertices[triangles[i][v]].z);
		}
	}
	glEnd();
}

void TriangleMesh::drawArray() {
	if (triangles.size() == 0) return;

  // We have to enable normal and vertex arrays
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // Since the Vec3<T> type is simple enough (no vtable etc.), we can treat
  // Vec3<T>[X] as T[X*3] and directly pass the normals and vertices
  // attributes to the corresponding GL functions

  glNormalPointer(
      GL_FLOAT,       // scalar type of vertices/normals
      sizeof(Normal), // memory offset between each vertex/normal
      normals.data()  // array of normal vertices
  );

  // same as above for vertices, 3 encodes the number of coordinates per vertex
  glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), vertices.data());

  // Instruct OpenGL to render the arrays specified above.
  // It allows us to define the order of vertices forming triangles without
  // repeating vertices by passing an index array.
  glDrawElements(
      GL_TRIANGLES,         // our vertices form triangles
      triangles.size() * 3, // the triangles vector stores 3 indices for every vector of each triangle -> number of indices
      GL_UNSIGNED_INT,      // our indices are unsigned integers
      triangles.data()      // pointer to the vertex index array
  );

  // We disable normal and vertex arrays again
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

void TriangleMesh::drawVBO() {

	GLuint i, j;
	glGenBuffers(1, &i);
	glGenBuffers(1, &j);

	if (VBOsupported == false) return;
	if (VBOv == 0 || VBOn == 0 || VBOf == 0) return;
	// TODO: draw in VBO mode

  // We have to enable normal and vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

  // We tell OpenGL that we want to work with the buffer where we stored the
  // normal data
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
  // We tell it, that this buffer contains the normals
	glNormalPointer(
      GL_FLOAT,       // scalars are floats
      sizeof(Normal), // offset to next normal is the size of a normal
      0               // normals begin at 0 offset in buffer
  );

  // We tell OpenGL that we want to work with the buffer where we stored the
  // normal data
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
  // We tell it, that this buffer contains the vertices
	glVertexPointer(
      3,              // our vertices have 3 coordinates
      GL_FLOAT,       // scalars are of float type
      sizeof(Vertex), // offset until next vertex is the size of a vertex
      0               // vertices begin with 0 offset in buffer
  );

  // we tell opengl, in which buffer indices are stored
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOf);

  // drawing everything
	glDrawElements(
      GL_TRIANGLES,         // we are drawing triangles
      triangles.size() * 3, // there are three entries per triangle in the index set
      GL_UNSIGNED_INT,      // indices are of unsigned integer type
      0                     // indices start at 0 offset in index buffer
  );

  // Unbind buffers and reset state.
  // Otherwise these settings would interfere with the other drawing modes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
