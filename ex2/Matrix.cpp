#include "Matrix.h"

std::Vec3f operator*(Matrix const & m, std::Vec3f const & v) {
  return {
    static_cast<float>(m.data[0]*v.x + m.data[1]*v.y + m.data[2]*v.z + m.data[3]*1),
    static_cast<float>(m.data[4]*v.x + m.data[5]*v.y + m.data[6]*v.z + m.data[7]*1),
    static_cast<float>(m.data[8]*v.x + m.data[9]*v.y + m.data[10]*v.z + m.data[11]*1)
  };
}

Matrix operator*(Matrix const & lhs, Matrix const & rhs) {
  return {
    lhs.data[0]*rhs.data[0] + lhs.data[1] * rhs.data[4] + lhs.data[2] * rhs.data[8],
      lhs.data[0]*rhs.data[1] + lhs.data[1] * rhs.data[5] + lhs.data[2] * rhs.data[9],
        lhs.data[0]*rhs.data[2] + lhs.data[1] * rhs.data[6] + lhs.data[2] * rhs.data[10],
          lhs.data[0]*rhs.data[3] + lhs.data[1] * rhs.data[7] + lhs.data[2] * rhs.data[11] + lhs.data[3],
    lhs.data[4]*rhs.data[0] + lhs.data[5] * rhs.data[4] + lhs.data[6] * rhs.data[8],
      lhs.data[4]*rhs.data[1] + lhs.data[5] * rhs.data[5] + lhs.data[6] * rhs.data[9],
        lhs.data[4]*rhs.data[2] + lhs.data[5] * rhs.data[6] + lhs.data[6] * rhs.data[10],
          lhs.data[4]*rhs.data[3] + lhs.data[5] * rhs.data[7] + lhs.data[6] * rhs.data[11] + lhs.data[7],
    lhs.data[8]*rhs.data[0] + lhs.data[9] * rhs.data[4] + lhs.data[10] * rhs.data[8],
      lhs.data[8]*rhs.data[1] + lhs.data[9] * rhs.data[5] + lhs.data[10] * rhs.data[9],
        lhs.data[8]*rhs.data[2] + lhs.data[9] * rhs.data[6] + lhs.data[10] * rhs.data[10],
          lhs.data[8]*rhs.data[3] + lhs.data[9] * rhs.data[7] + lhs.data[10] * rhs.data[11] + lhs.data[11],
  };
}
