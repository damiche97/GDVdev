#ifndef OBJECT_FORMATION
#define OBJECT_FORMATION

#include <vector>

#include "TriangleMesh.h"

/**
 * Loads the same model multiple times in a vector in the
 * given formation.
 *
 * See main.cpp for an usage example
 */
std::vector<TriangleMesh> loadFormation(
    const char * const modelPath, // path to model
    float modelScaling, // Factor to scale model
    std::vector<std::vector<bool>> formationGrid, // formation in which the model shall be replicated
    float cellSpacing, // space between adjacent models
    Vec3f const & translation // translation of whole resulting formation
);

#endif
