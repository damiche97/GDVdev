#include "BoundingPlanes.h"

void multiply(float left_mat[16], float right_mat[16], float result[16]) {
  // iterate over result array columns in outer loop, so that we fill result array in order
  for (unsigned result_col = 0; result_col < 4; ++result_col) {
    // iterate over result array rows
    for (unsigned result_row = 0; result_row < 4; ++result_row) {
      // the left matrix is multiplied row by row, so the starting index to
      // compute the current result cell is within the first column
      const auto left_start_idx = result_row;
      // since the matrices are stored in consecutive columns, 4 has to be added
      // to reach the next element in the current row of the left matrix
      // for the current summation step
      const constexpr auto left_step = 4;

      // the right matrix is multiplied column by column
      const auto right_start_idx = result_col * 4;
      // to reach the next element in the current summation step for the
      // right matrix, we can just increase the index by 1, since the
      // matrices are stored column-wise
      const constexpr auto right_step = 1;

      float sum = 0.f; // we will store the summation result here
      for (unsigned step = 0; step < 4; ++step) { // summation is done in 4 steps
        sum +=
            left_mat[left_start_idx + left_step * step]
          * right_mat[right_start_idx + right_step * step];
      }

      result[result_col * 4 + result_row] = sum;
    }
  }
}

void multiplyVector(float N[16], const float M[4], float Result[4]) {
	for (unsigned I = 0; I < 4; ++I) {
		float SumElements = 0.0f;

		for (unsigned vecIdx = 0; vecIdx < 4; ++vecIdx) {
			SumElements += M[vecIdx] * N[I + vecIdx*4];

      Result[I] = SumElements;
		}
	}
}

// get m_{row column} for a matrix M = (m_{ij}) in a safe way
template <std::size_t row, std::size_t column>
inline float getComp(std::array<float, 16> const & mat) {
  return std::get<(column-1) * 4 + (row-1)>(mat);
}

BoundingPlanes planesFromMatrices(
    float modelview_mat[16],
    float projection_mat[16]
) {
  // We extract the planes using https://web.archive.org/web/20170709073948/www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf

  // M = P * V, where V is the modelview matrix and P is the projection matrix
  std::array<float, 16> M;
  multiply(projection_mat, modelview_mat, M.data());

  // The coefficients a, b, c of the plane equation
  //
  // 0 < nv + d
  // where n = (a, b, c)
  //
  // can be computed from M = (m_{ij}) like this for the frustrum planes:

  return {{
    // left
    Plane {
      std::Vec3f {
        getComp<4,1>(M) + getComp<1,1>(M), // a = m_41 + m_11
        getComp<4,2>(M) + getComp<1,2>(M), // b = m_42 + m_12
        getComp<4,3>(M) + getComp<1,3>(M)  // c = m_43 + m_13
      },
      getComp<4,4>(M) + getComp<1,4>(M) // d = m_44 + m_14
    },

    // right
    Plane {
      std::Vec3f {
        getComp<4,1>(M) - getComp<1,1>(M),
        getComp<4,2>(M) - getComp<1,2>(M),
        getComp<4,3>(M) - getComp<1,3>(M)
      },
      getComp<4,4>(M) - getComp<1,4>(M)
    },

    // bottom
    Plane {
      std::Vec3f {
        getComp<4,1>(M) + getComp<2,1>(M),
        getComp<4,2>(M) + getComp<2,2>(M),
        getComp<4,3>(M) + getComp<2,3>(M)
      },
      getComp<4,4>(M) + getComp<2,4>(M)
    },

    // top
    Plane {
      std::Vec3f {
        getComp<4,1>(M) - getComp<2,1>(M),
        getComp<4,2>(M) - getComp<2,2>(M),
        getComp<4,3>(M) - getComp<2,3>(M)
      },
      getComp<4,4>(M) - getComp<2,4>(M)
    },

    // near
    Plane {
      std::Vec3f {
        getComp<4,1>(M) + getComp<3,1>(M),
        getComp<4,2>(M) + getComp<3,2>(M),
        getComp<4,3>(M) + getComp<3,3>(M)
      },
      getComp<4,4>(M) + getComp<3,4>(M)
    },

    // far
    Plane {
      std::Vec3f {
        getComp<4,1>(M) - getComp<3,1>(M),
        getComp<4,2>(M) - getComp<3,2>(M),
        getComp<4,3>(M) - getComp<3,3>(M)
      },
      getComp<4,4>(M) - getComp<3,4>(M)
    }
  }};
}

BoundingPlanes::BoundingPlanes(
    PlanesArray && planes
)
  : planes(std::move(planes))
{}

bool BoundingPlanes::isInside(std::Vec3f const & vec) const {
  return
       // between top and bottom
       getPlane<Top>().isInNormalDirection(vec) && getPlane<Bottom>().isInNormalDirection(vec)
       // and between left and right
    && getPlane<Left>().isInNormalDirection(vec) && getPlane<Right>().isInNormalDirection(vec)
       // and between near and far plane
    && getPlane<Far>().isInNormalDirection(vec) && getPlane<Near>().isInNormalDirection(vec);
}

/**
 * Returns vertices of a box given by its minimum and maximum values.
 * The following order is guaranteed:
 * 
 * 
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
std::array<Vec3f, 8> deriveBoxCorners(Vec3f const & min, Vec3f const & max) {
  return {
    Vec3f {min.x, min.y, min.z},
    Vec3f {max.x, min.y, min.z},
    Vec3f {min.x, max.y, min.z},
    Vec3f {min.x, min.y, max.z},

    Vec3f {max.x, max.y, max.z},
    Vec3f {min.x, max.y, max.z},
    Vec3f {max.x, min.y, max.z},
    Vec3f {max.x, max.y, min.z}
  };
}

std::pair<Vec3f, Vec3f> getPandNVec(
    Plane const & plane,
    Vec3f const & minVec,
    Vec3f const & maxVec
) {
  const Vec3f pVec {
    plane.normal.x >= 0 ?
      maxVec.x : minVec.x,

    plane.normal.y >= 0 ?
      maxVec.y : minVec.y,

    plane.normal.z >= 0 ?
      maxVec.z : minVec.z,
  };

  const Vec3f nVec {
    plane.normal.x >= 0 ?
      minVec.x : maxVec.x,

    plane.normal.y >= 0 ?
      minVec.y : maxVec.y,

    plane.normal.z >= 0 ?
      minVec.z : maxVec.z,
  };

  return {
    pVec, nVec
  };
}


/**
 * Culling using axis-aligned bounding boxes with a bounding volume defined by
 * 6 planes (view frustrum) using the technique described in
 *
 * https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
 */
bool BoundingPlanes::doOverlap(TriangleMesh const & mesh) const {
  const auto minVec = mesh.getBoundingBoxMin();
  const auto maxVec = mesh.getBoundingBoxMax();

  for (Plane const & plane : planes) {
    const auto [pVec, nVec] = getPandNVec(plane, minVec, maxVec);

    if (!plane.isInNormalDirection(pVec)) {
      return false;
    }
  }

  return true;
}

std::ostream & operator<<(std::ostream & out, BoundingPlanes const & bplanes) {
   return out << "LEFT " << bplanes.getPlane<BoundingPlanes::Left>() << "\n"
              << "RIGHT " << bplanes.getPlane<BoundingPlanes::Right>() << "\n"
              << "BOTTOM " << bplanes.getPlane<BoundingPlanes::Bottom>() << "\n"
              << "TOP " << bplanes.getPlane<BoundingPlanes::Top>() << "\n"
              << "NEAR " << bplanes.getPlane<BoundingPlanes::Near>() << "\n"
              << "FAR " << bplanes.getPlane<BoundingPlanes::Far>();
}

