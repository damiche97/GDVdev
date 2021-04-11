#ifndef SCENELOADER_H
#define SCENELOADER_H

#include <functional>
#include <optional>
#include <limits>

#include <GL/glew.h>      // openGL helper
#include <GL/glut.h>      // openGL helper

#include "TriangleMesh.h"
#include "main.h"

struct ModelScaling { float value; };
struct MatAmbient { float r; float g; float b; };
struct MatDiffuse { float r; float g; float b; };
struct MatSpecular { float r; float g; float b; };
struct MatReflectiveness { float r; float g; float b; };
struct MatOpacity { float r; float g; float b; };
struct MatRefractiveIndex { float value; };
struct MatShininess { float value; };
struct TextureID { std::size_t value; };

const auto NoTransform = [](TriangleMesh & mesh){};
const auto NoTexture = TextureID { std::numeric_limits<std::size_t>::max() };

void load(
  const char * modelPath,
  ModelScaling modelScaling,
  std::function<void (TriangleMesh &)> transformations,
  TextureID textureID,
  MatAmbient matAmbient,
  MatDiffuse matDiffuse,
  MatSpecular matSpecular,
  MatReflectiveness matReflectiveness = MatReflectiveness { 0.f, 0.f, 0.f },
  MatOpacity matOpacity = MatOpacity { 1.f, 1.f, 1.f },
  MatRefractiveIndex matRefractiveIndex = MatRefractiveIndex { 1.0f },
  MatShininess matShininess = MatShininess { 0.8f * 128.0f }
) {
  TriangleMesh tm;
  tm.loadOFF(modelPath, Vec3f(0.0f,0.0f,0.0f), modelScaling.value);
  transformations(tm);
  meshes.push_back(tm);

  SceneObject so;
  so.matAmbient[0]        = matAmbient.r;        so.matAmbient[1]        = matAmbient.g;        so.matAmbient[2]        = matAmbient.b; so.matAmbient[3]        = 1.0f;
  so.matDiffuse[0]        = matDiffuse.r;        so.matDiffuse[1]        = matDiffuse.g;        so.matDiffuse[2]        = matDiffuse.b; so.matDiffuse[3]        = 1.0f;
  so.matSpecular[0]       = matSpecular.r;       so.matSpecular[1]       = matSpecular.g;       so.matSpecular[2]       = matSpecular.b; so.matSpecular[3]       = 1.0f;
  so.matReflectiveness[0] = matReflectiveness.r; so.matReflectiveness[1] = matReflectiveness.g; so.matReflectiveness[2] = matReflectiveness.b; so.matReflectiveness[3] = 1.0f;
  so.matOpacity[0]        = matOpacity.r;        so.matOpacity[1]        = matOpacity.g;        so.matOpacity[2]        = matOpacity.b;  so.matOpacity[3]        = 1.f;
  so.matRefractiveIndex = matRefractiveIndex.value;
  so.matShininess = matShininess.value;

  if (textureID.value == std::numeric_limits<std::size_t>::max()) {
    so.textureID = std::optional<GLuint>{};
  }

  else {
    so.textureID = std::make_optional(textureID.value);
  }

  objects.push_back(so);
}

#endif //SCENELOADER_H
