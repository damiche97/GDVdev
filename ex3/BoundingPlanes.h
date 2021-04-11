#ifndef BOUNDING_PLANES_H
#define BOUNDING_PLANES_H

#include "Plane.h"
#include "TriangleMesh.h"
#include <array>
#include <iostream>

class BoundingPlanes {
  /**
   * An array of 6 planes defines a bounding volume.
   * The planes shall be ordered as illustrated below
   * (see also SideId enum and getPlane function).
   *
   * bounding volume defined by 6 planes with normals pointing inside:
                                        top
                                        \/
                    5 - - - - - - - - - 2
                    | \                 | \
                    |   \               |   \
                    |     \             |     \
                    |       \    far    |       \
                    |         \         |         \
                    |           \       |           \
                    |             4 - - - - - - - - - 7
                    |             |     |             |
         left>      |             |     |             |    <right
                    3 - - - - - - | - - 0             |
                      \           |       \           |
                        \         |       near        |
                          \       |           \       |
                            \     |             \     |             Y+
                              \   |               \   |             |
                                \ |                 \ |         Z+ --
                                  6 - - - - - - - - - 1               \
                                                                      +X

                                  /\
                                bottom
   */
  public:
    enum SideId: std::size_t {
      Left = 0,
      Right = 1,
      Bottom = 2,
      Top = 3,
      Near = 4,
      Far = 5
    };

  private:
    using PlanesArray = std::array<Plane, 6>;
    PlanesArray planes;

  public:
    /**
     * The constructor expects planes in this order:
     *
     * LEFT
     * RIGHT
     * BOTTOM
     * TOP
     * NEAR
     * FAR
     */
    BoundingPlanes(
      PlanesArray && planes
    );

    bool isInside(std::Vec3f const & vec) const;
    bool doOverlap(TriangleMesh const & mesh) const;

    /**
     * Get a certain plane by its name in a safe way
     **/
    template <SideId id>
    Plane const & getPlane() const {
      return std::get<
        static_cast<std::size_t>(id)
      >(planes);
    }
};


BoundingPlanes planesFromMatrices(
    float modelview_mat[16],
    float projection_mat[16]
);

void multiply(float left_mat[16], float right_mat[16], float result[16]);
void multiplyVector(float N[16], const float M[4], float Result[4]);

std::ostream & operator<<(std::ostream & out, BoundingPlanes const & bplanes);

#endif
