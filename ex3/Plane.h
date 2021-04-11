#ifndef PLANE_H
#define PLANE_H

#include "Vec3.h"
#include <array>
#include <iostream>

class Plane {
  public:
    std::Vec3f normal;
    float dist0;

    Plane(
      std::Vec3f normal,
      float dist0
    );

    bool isInNormalDirection(std::Vec3f const & vec) const;

    /**
     * This function assumes, that the difference of the two points form the
     * normal of the plane and the first point is on the plane
     */
    static Plane fromTwoPoints(std::Vec3f const & a, std::Vec3f const & b);
};

std::ostream & operator<<(std::ostream & lhs, Plane const & rhs);

#endif
