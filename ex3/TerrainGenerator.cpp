#include "TerrainGenerator.h"

#include <array>
#include <iostream>
#include <algorithm>


TerrainGenerator::TerrainGenerator(
  std::size_t wPoints,
  std::size_t hPoints,
  float pointDist
): wPoints{wPoints}, hPoints{hPoints}, pointDist{pointDist},
   width {wPoints * pointDist},
   height {hPoints * pointDist}
{ }

float TerrainGenerator::getRandFloat() {
  return uniform_dist2(generator);
}

float TerrainGenerator::getZeroOneFloat() {
  return zero_one_dist(generator);
}

std::vector<Vec3f> TerrainGenerator::genPlaneGrid(float initialY) const {
  std::vector<Vec3f> grid;
  grid.reserve(wPoints * hPoints);
  for (std::size_t i = 0; i < wPoints; ++i) {
    for (std::size_t j = 0; j < hPoints; ++j) {
      const float x = i * pointDist - width / 2;
      const float z = j * pointDist - height / 2;

      grid.emplace_back(
          x, initialY, z
      );
    }
  }

  return grid;
}

float TerrainGenerator::disp(std::size_t divisionIteration, std::size_t numDivisionIterations) const {
  const auto disp0 = 400.0f * pointDist / numDivisionIterations; // smaller distances between points or more divisions = smaller disp steps
  const auto dispN = disp0 / numDivisionIterations; // disp decrease depends on initial disp and number of divisions

  if (divisionIteration < numDivisionIterations) {
    return disp0 + (divisionIteration / numDivisionIterations) * ( dispN - disp0);
  }

  else {
    return dispN;
  }
}

float steepFunction(float dist, float disp) {
  if (dist > 0) {
    return disp;
  }

  else {
    return -disp;
  }
}

float sineFunction(float waveSize, float dist, float disp) {
  const auto criticalDist = M_PI / (2*waveSize);

  if (dist > criticalDist) {
    return disp;
  }

  else if (dist < -criticalDist) {
    return -disp;
  }

  else {
    return disp*sin((M_PI*dist)/(2*waveSize));
  }
}

void TerrainGenerator::applyDisplacements(
    std::vector<Vec3f> & grid
) {
  // make number of divisions depend on size of grid and distance between points.
  // => More divisions for bigger grids and point distances
  const auto numDivisionIterations = (wPoints * hPoints) * 0.03 * pointDist;

  // Divide grid by line `numDivisionIterations` times
  for (std::size_t divisionIteration = 0; divisionIteration < numDivisionIterations; ++divisionIteration) {
    const auto v = getRandFloat();
    const auto a = sin(v);
    const auto b = cos(v);

    const auto dist = sqrt(width * width + height * height);
    const auto c = getZeroOneFloat() * dist - dist / 2;

    for (std::size_t i = 0; i < wPoints; ++i) {
      for (std::size_t j = 0; j < hPoints; ++j) {
        const auto dispi = disp(divisionIteration, numDivisionIterations);

        auto & vec = grid[vecIdx(i, j)];
        const auto distFromDivision = a * vec.x + b * vec.z - c;

        vec.y += sineFunction(1.0, distFromDivision, dispi);
      }
    }
  }
}

std::size_t TerrainGenerator::vecIdx(std::size_t x, std::size_t z) const {
  return z * wPoints + x;
}

std::vector<Vec3ui> TerrainGenerator::genTriangles() const {
  std::vector<Vec3ui> triangles;
  triangles.reserve((wPoints - 1)*2 + (hPoints - 1));

  /*
     Divides the grid like this into triangles:

     x__x__x__x
     | /| /| /|
     |/ |/ |/ |
     x--x--x--x
     | /| /| /|
     |/ |/ |/ |
     x--x--x--x

     Where the "x" are the grid vertices
  */

  for (std::size_t z = 0; z < hPoints - 1; ++z) {
    for (std::size_t x = 0; x < wPoints - 1; ++x) {
      triangles.emplace_back(
        vecIdx(x, z),
        vecIdx(x+1, z),
        vecIdx(x+1, z+1)
      );

      triangles.emplace_back(
        vecIdx(x, z),
        vecIdx(x+1, z+1),
        vecIdx(x, z+1)
      );
    }
  }

  return triangles;
}

void TerrainGenerator::genColorPerVertex(TriangleMesh & mesh, std::function<TriangleMesh::Color (TriangleMesh::Vertex const &)> genFun) const {
  // create a vector to store colors in
  std::vector<TriangleMesh::Color> colors;
  colors.reserve(mesh.getVertices().size());

  // map `genFun` to each vertex...
  std::transform(
      std::begin(mesh.getVertices()),
      std::end(mesh.getVertices()),
      std::back_inserter(colors), //... and store the resulting color in the storage defined above
      genFun
  );

  // enable usage of colors in the mesh
  mesh.setWithColorArray(true);
  // efficiently move the color storage memory defined above into the mesh object
  mesh.setColors(std::move(colors));
}

std::pair<float, float> TerrainGenerator::getMinMaxY(TriangleMesh const & mesh) const {
  const auto [min, max] = std::minmax_element(
      std::begin(mesh.getVertices()),
      std::end(mesh.getVertices()),
      [](auto const & lhs, auto const & rhs) {
        return lhs.y < rhs.y;
      }
  );

  return {min->y, max->y};
}

void TerrainGenerator::applyColors(TriangleMesh & mesh) const {
  const auto [min, max] = getMinMaxY(mesh);
  const auto difference = max - min;

  // define color borders as relative height
  const auto whiteColorLimit = min + difference * 0.8;
  const auto grayColorLimit = min + difference * 0.7;
  const auto lightGreenColorLimit = min + difference * 0.5;

  // define colors to be used for terrain
  const TriangleMesh::Color white {0.98, 0.98, 0.98};
  const TriangleMesh::Color gray {0.741, 0.741, 0.741};
  const TriangleMesh::Color lightGreen {0.545, 0.765, 0.29};
  const TriangleMesh::Color darkGreen {0.2, 0.412, 0.118};

  // generate a color for each vertex
  genColorPerVertex(
      mesh,
      [&](TriangleMesh::Vertex const & vec) {
        // select the correct color depending on the current vertex' y value
        if (vec.y >= whiteColorLimit) {
          return white;
        }

        else if (vec.y >= grayColorLimit) {
          return gray;
        }

        else if (vec.y >= lightGreenColorLimit) {
          return lightGreen;
        }

        else {
          return darkGreen;
        }
      }
  );
}

TriangleMesh TerrainGenerator::genTerrain() {
  auto grid = genPlaneGrid();
  applyDisplacements(grid);

  auto triangles = genTriangles();

  TriangleMesh mesh;
  mesh.loadFromMemory(
      std::move(grid),
      std::move(triangles)
  );

  applyColors(mesh);

  return mesh;
}

TriangleMesh TerrainGenerator::genWater(TriangleMesh const & terrain) {
  const auto [min, max] = getMinMaxY(terrain);
  const auto difference = max - min;

  // generate a flat plane at the relative height of 30% in the terrain
  const auto waterPlaneY = min + difference * 0.3f;
  std::vector<TriangleMesh::Vertex> grid {
    {-width / 2, waterPlaneY, -height/2},
    {width / 2, waterPlaneY, -height/2},
    {width / 2, waterPlaneY, height/2},
    {-width / 2, waterPlaneY, height/2}
  };
  std::vector<TriangleMesh::Triangle> triangles {
    {0, 1, 2},
    {0, 2, 3}
  };

  // load the plane into a mesh object
  TriangleMesh mesh;
  mesh.loadFromMemory(
      std::move(grid),
      std::move(triangles)
  );

  // color all vertices blue
  genColorPerVertex(
      mesh,
      [](TriangleMesh::Vertex const & vec) {
        return TriangleMesh::Color {0.161, 0.475, 1};
      }
  );

  return mesh;
}
