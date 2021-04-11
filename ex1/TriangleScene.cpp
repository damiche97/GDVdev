#include "TriangleMesh.h"
#include "Vec3.h"

#include <vector>
#include <random>

struct TriangleStart {
  Vec3f v1;
  Vec3f v2;
};

/**
 * Random generation of triangles.
 * Will generate num_base_triangles*3 triangles.
 *
 * It starts with a single edge and appends a equilateral triangle.
 * Then again an edge is randomly selected and the process repeated.
 * Additional elevated points are inserted at the center of the generated
 * triangles forming sub-triangles. This way, the generated structure isnt
 * completely flat.
 */
void triangleScene(TriangleMesh & mesh, std::size_t num_base_triangles) {
  // side length of triangles
  const float dist = 0.2f;

  // initial side
  TriangleStart nextStart {
    Vec3f {0.0, 0.0, 0.0},
    Vec3f {0.0, dist, 0.0}
  };

  // source of randomness
  std::random_device r;
  std::mt19937 g(r()); // Pseudo-random number generator
  // Uniform distribution between 0 and dist*0.8 for the elevation of the center
  // points
  std::uniform_real_distribution<> zVariationDistribution(0, dist*0.8f);
  // Uniform distribution between 0 and 1.0 to which will be used to randomly
  // decide on which edge the next triangle shall be added.
  std::uniform_real_distribution<> additionDistribution(0, 1.0);

  // Initial probability of appending a triangle on the "left" side.
  float additionBias = 0.5;
  // unit vector to be multiplied with the random elevation and to be added to the
  // triangle center points
  const Vec3f zVariation {0.0, 0.0, 1.0};

  // loop until we generated enough triangles
  for (std::size_t i = 0; i < num_base_triangles; ++i) {
    // vector between current base triangle side we are appending to
    auto startLine = nextStart.v2 - nextStart.v1;

    // normal to that side in x-y plane
    Vec3f startNormal { startLine.y, -startLine.x, 0};
      startNormal.normalize();
      // We select the length of that normal matching the height of the triangle
      // so that it is equilateral
      startNormal *= (sqrt(3)*dist)/2;

    // point to generate new base triangle
    auto newPoint = 
      // We jump from v1 to the middle point between v1 and v2 and then add the
      // normal:
      (nextStart.v2 - nextStart.v1) / 2 + startNormal + nextStart.v1;

    // center point of base triangle, which we might lift up a bit
    auto centerPoint =
      (nextStart.v2 -nextStart.v1) / 2 + startNormal / 2 + nextStart.v1
      + zVariation * zVariationDistribution(g); // Applying random elevation

    // build sub triangles with elevated centerpoint
    mesh.add(nextStart.v1, nextStart.v2, centerPoint);
    mesh.add(nextStart.v1, newPoint, centerPoint);
    mesh.add(nextStart.v2, newPoint, centerPoint);

    // randomly decide, at which side we will continue next
    if (additionDistribution(g) <= additionBias) {
      nextStart = TriangleStart {nextStart.v1, newPoint};
      // change probabilities to make it more likely to choose the other side next
      additionBias = 0.3;
    }

    else {
      nextStart = TriangleStart {newPoint, nextStart.v2};
      additionBias = 0.7;
    }
  }

  // Calculate edge normals to enable correct lighting of the triangle mesh
  mesh.calculateNormals();
}
