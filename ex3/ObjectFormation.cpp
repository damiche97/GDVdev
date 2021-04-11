#include "ObjectFormation.h"

std::vector<TriangleMesh> loadFormation(
    const char * const modelPath,
    float modelScaling,
    std::vector<std::vector<bool>> formationGrid,
    float cellSpacing,
    Vec3f const & translation
) {
  // We will store all resulting models here
  std::vector<TriangleMesh> result;

  // Load base model
  TriangleMesh originalModel;
  originalModel.loadOFF(modelPath, {0.f, 0.f, 0.f}, modelScaling);

  // Get minimum and maximum values for all coordinates of the model
  const auto modelMax = originalModel.getBoundingBoxMax();
  const auto modelMin = originalModel.getBoundingBoxMin();

  // we will use them to calculate how much space we need to leave between cells
  // of the formation at minimum, so that models do not intersect
  const auto xStep = modelMax.x - modelMin.x + cellSpacing;
  const auto zStep = modelMax.z - modelMin.z + cellSpacing;

  // We will accumulate translation distances in these variables while iterating
  // over the formation cells
  float xTranslation = 0;
  float zTranslation = 0;

  // Iterate over all cells in the formation specification
  for (const auto & row : formationGrid) {
    for (bool placeModelInCell : row) {
      if (placeModelInCell) {
        // If the cell is set, place a copy of the original model in the
        // result vector
        result.push_back(originalModel);

        // And translate the copy according to its position in the formation
        result.back().translateToCenter(
            translation + Vec3f {xTranslation, 0.f, zTranslation}
        );
      }

      xTranslation += xStep;
    }
    xTranslation = 0;

    zTranslation += zStep;
  }

  return result;
}
