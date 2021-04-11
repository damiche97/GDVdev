#ifndef MATRIX_H
#define MATRIX_H

#include "Vec3.h"

/**
 * Represents 4d transformation matrix.
 * Does not store 4th row, since it is always {0, 0, 0, 1} for our transformations
 */
class Matrix {
  public:
    double data[12];
};

// multiply matrix with vector
std::Vec3f operator*(Matrix const & m, std::Vec3f const & v);

// multiply matrix with matrix
Matrix operator*(Matrix const & lhs, Matrix const & rhs);

#endif
