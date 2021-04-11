#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <random>
#include <functional>

#include "TriangleMesh.h"

class TerrainGenerator {
  private:
    const std::size_t wPoints;
    const std::size_t hPoints;
    const float pointDist;

    // Width and height of terrain
    const float width;
    const float height;

    // Randomness sources
    std::mt19937 generator { std::random_device{}() };
    std::uniform_real_distribution<float> zero_one_dist { 0, 1 };
    std::uniform_real_distribution<float> uniform_dist2 { -100.0, 100.0 };

    /**
     * Get uniformly distributed number between -100 and 100
     */
    float getRandFloat();

    /**
     * Get uniformly distributed number in [0; 1[
     */
    float getZeroOneFloat();

    /**
     * Helper function which computes an index in the continuous array storing
     * the grid, for grid xz coordinates.
     */
    std::size_t vecIdx(std::size_t x, std::size_t z) const;

    /**
     * Generates a plain grid of vectors in the xz-plane
     *
     * @param initialY initial y position of all points of the plane
     */
    std::vector<Vec3f> genPlaneGrid(float initialY = 0.f) const;

    /**
     * Generates a vector of triangles, which are references to vector indices.
     */
    std::vector<Vec3ui> genTriangles() const;

    /**
     * Computes, how far the 2 parts of the terrain grid should be displaced
     * from one another in y direction.
     */
    float disp(std::size_t divisionIteration, std::size_t numDivisionIterations) const;

    /**
     * Perform the actual terrain generation by displacing parts of the grid in
     * the y direction
     */
    void applyDisplacements(
        std::vector<Vec3f> & grid
    );

    /**
     * Colors terrain according to height:
     * Snowy mountain tops, green valleys etc.
     */
    void applyColors(TriangleMesh & mesh) const;

    /**
     * Helper function, which generates colors for each vertex of a mesh.
     *
     * @param mesh mesh for which colors shall be generated
     * @param genFun function which generates a color for a given vertex
     */
    void genColorPerVertex(TriangleMesh & mesh, std::function<TriangleMesh::Color (TriangleMesh::Vertex const &)> genFun) const;

    /**
     * Returns the minimum and maximum y values of a mesh.
     */
    std::pair<float, float> getMinMaxY(TriangleMesh const & mesh) const;

  public:
    TerrainGenerator(
      std::size_t wPoints,
      std::size_t hPoints,
      float pointDist
    );

    TriangleMesh genTerrain();
    TriangleMesh genWater(TriangleMesh const & terrain);
};

#endif
